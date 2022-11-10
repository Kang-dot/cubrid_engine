/*
 * Copyright 2008 Search Solution Corporation
 * Copyright 2016 CUBRID Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef _ATOMIC_REPLICATION_HELPER_HPP_
#define _ATOMIC_REPLICATION_HELPER_HPP_

#include <map>
#include <vector>
#include <set>
#include <sstream>

#include "log_lsa.hpp"
#include "log_record.hpp"
#include "log_recovery_redo.hpp"
#include "page_buffer.h"
#include "thread_entry.hpp"
#include "vpid_utilities.hpp"

// various local checks; currently linked to the debug state (to be removed at some point)
#if !defined (NDEBUG)
#   define ATOMIC_REPL_PAGE_BELONGS_TO_SINGLE_ATOMIC_SEQUENCE_CHECK
#   define ATOMIC_REPL_PAGE_PTR_BOOKKEEPING_DUMP
#endif

namespace cublog
{
  /*
   * Helper class that provides methods to allow handling of all atomic replication sequences
   * generated on the active transaction server.
   *
   * For all scenarios details below, the following rules apply:
   * - from the helper's point of view, there are two types of log records:
   *  - control log records - see 'is_control'
   *  - proper log records
   * - control log records are also added to the atomic sequence as atomic log entries
   *    and they help in deciding for more advanced scenarios (and provide for future extensions)
   * - in-between control records are the proper log records
   * - a new control record is added to the sequence, all proper log records are applied and
   *    the page handles are released (the bookkeeping implementation takes care of this)
   *
   * The following cases are covered by the implementation:
   *
   * (1)
   *    user transaction, explicit atomic sequences
   *
   *    LOG_START_ATOMIC_REPL
   *        (undo|redo|undoredo|..)+
   *    LOG_END_ATOMIC_REPL
   *
   * (2)
   *    standalone sysop atomic sequences whose ending condition is based on the sysop end's
   *    last parent LSA
   *
   *    LOG_SYSOP_ATOMIC_START
   *        (undo|redo|undoredo|..)+
   *    LOG_SYSOP_END with LOG_SYSOP_END_COMMIT
   *
   * (3)
   *    vacuum generated atomic sysops with nested postpone logical operations which, themselves,
   *    can contain atomic operations; these have the following layout:
   *
   *    LOG_SYSOP_ATOMIC_START
   *        (undo|redo|undoredo|..)+
   *        ..
   *        LOG_POSTPONE 1 (eg: with RVFL_DEALLOC - logical or physical change)
   *        (undo|redo|undoredo|..)*
   *        ..
   *        LOG_POSTPONE 2 (eg: other index)
   *        (undo|redo|undoredo|..)*
   *        ..
   *    LOG_SYSOP_START_POSTPONE
   *        log records for LOG_POSTPONE 1 (RVFL_DEALLOC) with postpone_lsa pointing to the
   *          LSA of LOG_POSTPONE 1
   *        LOG_SYSOP_END with LOG_SYSOP_END_LOGICAL_RUN_POSTPONE
   *        log records for LOG_POSTPONE 2 (other index) with postpone_lsa pointing *still* to the
   *          LSA of LOG_POSTPONE 1 (yes, it points to the start of all postpones, to allow recovery
   *          which, potentially needs to iterate and execute all postpones if not already executed)
   *        LOG_SYSOP_END with LOG_SYSOP_END_LOGICAL_RUN_POSTPONE
   *    LOG_SYSOP_END with LOG_SYSOP_END_COMMIT
   *
   * (4)
   *    use transaction explicit atomic sequence "hosting" one or more sysop's; the first
   *    LOG_SYSOP_ATOMIC_START is actually two LOG_SYSOP_ATOMIC_START but there is an optimization
   *    in place to not add two consecutive log records of this type; special handling is
   *    implemented for this case
   *
   *    LOG_START_ATOMIC_REPL
   *      LOG_SYSOP_ATOMIC_START
   *        .. redo records ..
   *      LOG_SYSOP_END (with LOG_SYSOP_END_LOGICAL_UNDO )
   *        .. redo records ..
   *      LOG_SYSOP_END (with LOG_SYSOP_END_COMMIT)
   *        .. redo records ..
   *    LOG_END_ATOMIC_REPL
   */
  class atomic_replication_helper
  {
    public:
      atomic_replication_helper () = default;

      atomic_replication_helper (const atomic_replication_helper &) = delete;
      atomic_replication_helper (atomic_replication_helper &&) = delete;

      ~atomic_replication_helper () = default;

      atomic_replication_helper &operator= (const atomic_replication_helper &) = delete;
      atomic_replication_helper &operator= (atomic_replication_helper &&) = delete;

      // add a new log record as part of an already existing atomic replication
      // sequence (be it sysop or non-sysop)
      int append_log (THREAD_ENTRY *thread_p, TRANID tranid, LOG_LSA lsa,
		      LOG_RCVINDEX rcvindex, VPID vpid);

      bool is_part_of_atomic_replication (TRANID tranid) const;
      bool all_log_entries_are_control (TRANID tranid) const;

      LOG_LSA get_the_lowest_start_lsa () const;

      void append_control_log (
	      THREAD_ENTRY *thread_p, TRANID trid, LOG_RECTYPE rectype, LOG_LSA lsa,
	      const log_rv_redo_context &redo_context);
      void append_control_log_sysop_end (
	      THREAD_ENTRY *thread_p, TRANID trid, LOG_LSA lsa, LOG_SYSOP_END_TYPE sysop_end_type,
	      LOG_LSA sysop_end_last_parent_lsa);

      void forcibly_remove_sequence (TRANID trid);

    private: // methods
      void start_sequence_internal (TRANID trid, LOG_LSA start_lsa, const log_rv_redo_context &redo_context);
#ifdef ATOMIC_REPL_PAGE_BELONGS_TO_SINGLE_ATOMIC_SEQUENCE_CHECK
      bool check_for_page_validity (VPID vpid, TRANID tranid) const;
#endif
      void dump (const char *message) const;

    private: // types
      class atomic_log_sequence
      {
	public:
	  atomic_log_sequence () = delete;
	  explicit atomic_log_sequence (const log_rv_redo_context &redo_context);

	  atomic_log_sequence (const atomic_log_sequence &) = delete;
	  atomic_log_sequence (atomic_log_sequence &&) = delete;

	  ~atomic_log_sequence ();

	  atomic_log_sequence &operator= (const atomic_log_sequence &) = delete;
	  atomic_log_sequence &operator= (atomic_log_sequence &&) = delete;

	  // technical: function is needed to avoid double constructing a redo_context - which is expensive -
	  // upon constructing a sequence
	  void initialize (TRANID trid, LOG_LSA start_lsa);

	  int append_log (THREAD_ENTRY *thread_p, LOG_LSA lsa, LOG_RCVINDEX rcvindex, VPID vpid);

	  void apply_and_unfix (THREAD_ENTRY *thread_p);

	  LOG_LSA get_start_lsa () const;

	  void append_control_log (LOG_RECTYPE rectype, LOG_LSA lsa);
	  void append_control_log_sysop_end (
		  LOG_LSA lsa, LOG_SYSOP_END_TYPE sysop_end_type, LOG_LSA sysop_end_last_parent_lsa);

	  bool all_log_entries_are_control () const;
	  bool can_purge ();

	  void dump (const char *message) const;
	  void dump_to_buffer (char *&buf_ptr, int &buf_len) const;

	private: // types
	  /*
	   * Holds the log record information necessary for recovery redo
	   */
	  struct atomic_log_entry
	  {
	    atomic_log_entry () = delete;
	    atomic_log_entry (LOG_LSA lsa, VPID vpid, LOG_RCVINDEX rcvindex, PAGE_PTR page_ptr);
	    atomic_log_entry (LOG_LSA lsa, LOG_RECTYPE rectype);
	    atomic_log_entry (LOG_LSA lsa, LOG_SYSOP_END_TYPE sysop_end_type, LOG_LSA sysop_end_last_parent_lsa);

	    atomic_log_entry (const atomic_log_entry &) = delete;
	    atomic_log_entry (atomic_log_entry &&that);

	    atomic_log_entry &operator= (const atomic_log_entry &) = delete;
	    atomic_log_entry &operator= (atomic_log_entry &&that);

	    void apply_log_redo (THREAD_ENTRY *thread_p, log_rv_redo_context &redo_context) const;
	    template <typename T>
	    void apply_log_by_type (THREAD_ENTRY *thread_p, log_rv_redo_context &redo_context,
				    LOG_RECTYPE rectype) const;

	    inline bool is_control () const;

	    void dump_to_buffer (char *&buf_ptr, int &buf_len) const;
	    void dump_to_stream (std::stringstream &dump_stream) const;

	    VPID m_vpid;
	    LOG_RECTYPE m_rectype;
	    LOG_LSA m_lsa;
	    LOG_RCVINDEX m_rcvindex;

	    // only meaningful when dealing with a sysop end log record
	    LOG_SYSOP_END_TYPE m_sysop_end_type;
	    LOG_LSA m_sysop_end_last_parent_lsa;

	    // ownership of page pointer is with the bookkeeper in the owning class; this is just a
	    // reference to allow applying the redo function when needed
	    PAGE_PTR m_page_ptr;
	  };

	  using page_ptr_watcher_uptr_type = std::unique_ptr<PGBUF_WATCHER>;

	  struct page_ptr_info
	  {
	    page_ptr_info () = default;

	    page_ptr_info (const page_ptr_info &) = delete;
	    page_ptr_info (page_ptr_info &&) = default;

	    page_ptr_info &operator= (const page_ptr_info &) = delete;
	    page_ptr_info &operator= (page_ptr_info &&) = delete;

	    ~page_ptr_info ();

	    VPID m_vpid = VPID_INITIALIZER;
	    LOG_RCVINDEX m_rcvindex = RV_NOT_DEFINED;
	    PAGE_PTR m_page_p = nullptr;
	    page_ptr_watcher_uptr_type m_watcher_p;
	    int m_ref_count = -1;
	  };

	  /*
	   * Implements a RAII-like reference counted functionality to bookkeep page pointers for
	   * a sequence of [possibly] nested atomic replication sub-sequences.
	   * A page can be needed by multiple levels of a nested atomic replication sequence which
	   * perfom changes on the page. Once the page is unfixed in a sequence at a certain
	   * level, it can be:
	   *  - either still kept fixed if a parent [sub]sequence did the fixing and still
	   *    needs the page
	   *  - or unfixed if there is no parent [sub]sequence which needs the page anymore (aka:
	   *    the [sub]sequence which just requested the fix is the outer-most one that needed
	   *    the page in the current overall sequence of possibly nested [sub]sequences
	   */
	  struct page_ptr_bookkeeping
	  {
	    page_ptr_bookkeeping () = default;
	    ~page_ptr_bookkeeping ();

	    page_ptr_bookkeeping (const page_ptr_bookkeeping &) = delete;
	    page_ptr_bookkeeping (page_ptr_bookkeeping &&) = delete;

	    page_ptr_bookkeeping &operator= (const page_ptr_bookkeeping &) = delete;
	    page_ptr_bookkeeping &operator= (page_ptr_bookkeeping &&) = delete;

	    int fix_page (THREAD_ENTRY *thread_p, VPID vpid, LOG_RCVINDEX rcvindex, PAGE_PTR &page_ptr_out);
	    int unfix_page (THREAD_ENTRY *thread_p, VPID vpid);

#ifdef ATOMIC_REPL_PAGE_PTR_BOOKKEEPING_DUMP
	    void dump () const;
#endif

	    using page_ptr_info_map_type = std::map<VPID, page_ptr_info>;

	    page_ptr_info_map_type m_page_ptr_info_map;
	  };

	  using atomic_log_entry_vector_type = std::vector<atomic_log_entry>;

	private: // variables
	  /* the transaction this sequence belongs to; for logging/debugging purposes only */
	  TRANID m_trid;
	  /* The LSA of the log record which started this atomic sequence.
	   * It is used for comparison to see whether a sysop end operation can close an
	   * atomic replication sequence. */
	  LOG_LSA m_start_lsa;

	  log_rv_redo_context m_redo_context;
	  atomic_log_entry_vector_type m_log_vec;
	  page_ptr_bookkeeping m_page_ptr_bookkeeping;

	  // temporary mechanism to log all the log entries that were part of the sequence
	  std::stringstream m_full_dump_stream;
      };

      using sequence_map_type = std::map<TRANID, atomic_log_sequence>;

#ifdef ATOMIC_REPL_PAGE_BELONGS_TO_SINGLE_ATOMIC_SEQUENCE_CHECK
      // check validity of atomic sequences
      // one page can only be accessed by one atomic sequence within one transaction
      // this check makes sense because, on active transaction server, there is no
      // notion of an "atomic" sequence and, hence, it is totally possible that
      // another transaction might access the same page
      using vpid_set_type = std::set<VPID>;
#endif

    private: // variables
      sequence_map_type m_sequences_map;

#ifdef ATOMIC_REPL_PAGE_BELONGS_TO_SINGLE_ATOMIC_SEQUENCE_CHECK
      std::map<TRANID, vpid_set_type> m_vpid_sets_map;
#endif
  };

  /*********************************************************************************************************
   * standalone functions declarations
   *********************************************************************************************************/

  int pgbuf_fix_or_ordered_fix (THREAD_ENTRY *thread_p, VPID vpid, LOG_RCVINDEX rcvindex,
				std::unique_ptr<PGBUF_WATCHER> &watcher_uptr, PAGE_PTR &page_ptr);
  void pgbuf_unfix_or_ordered_unfix (THREAD_ENTRY *thread_p, LOG_RCVINDEX rcvindex,
				     std::unique_ptr<PGBUF_WATCHER> &watcher_uptr, PAGE_PTR &page_ptr);

  /*********************************************************************************************************
   * template/inline implementations
   *********************************************************************************************************/

  template <typename T>
  void
  atomic_replication_helper::atomic_log_sequence::atomic_log_entry::apply_log_by_type (
	  THREAD_ENTRY *thread_p, log_rv_redo_context &redo_context, LOG_RECTYPE rectype) const
  {
    LOG_RCV rcv;
    assert (m_page_ptr != nullptr);
    rcv.pgptr = m_page_ptr;

    redo_context.m_reader.advance_when_does_not_fit (sizeof (T));
    const log_rv_redo_rec_info<T> record_info (m_lsa, rectype,
	redo_context.m_reader.reinterpret_copy_and_add_align<T> ());
    if (log_rv_check_redo_is_needed (rcv.pgptr, record_info.m_start_lsa, redo_context.m_end_redo_lsa))
      {
	rcv.reference_lsa = m_lsa;
	log_rv_redo_record_sync_apply (thread_p, redo_context, record_info, m_vpid, rcv);
      }
  }

  inline bool
  atomic_replication_helper::atomic_log_sequence::atomic_log_entry::is_control () const
  {
    return (m_rectype == LOG_START_ATOMIC_REPL ||
	    m_rectype == LOG_END_ATOMIC_REPL ||
	    m_rectype == LOG_SYSOP_ATOMIC_START ||
	    m_rectype == LOG_SYSOP_END ||
	    m_rectype == LOG_SYSOP_START_POSTPONE);
  }
}

#endif // _ATOMIC_REPLICATION_HELPER_HPP_
