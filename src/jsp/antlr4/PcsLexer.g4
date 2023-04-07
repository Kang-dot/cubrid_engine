/**
 * CUBRID PL/CSQL Parser grammar based on and updated from
 *  Oracle(c) PL/SQL 11g Parser (https://github.com/antlr/grammars-v4/tree/master/sql/plsql)
 *
 * Copyright (c) 2009-2011 Alexandre Porcelli <alexandre.porcelli@gmail.com>
 * Copyright (c) 2015-2019 Ivan Kochurkin (KvanTTT, kvanttt@gmail.com, Positive Technologies).
 * Copyright (c) 2017 Mark Adams <madams51703@gmail.com>
 * Copyright (c) 2016 CUBRID Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

lexer grammar PcsLexer;

@header {
package com.cubrid.plcsql.compiler.antlrgen;
}

A_LETTER:                     A ;
ALL:                          A L L ;
AND:                          A N D ;
ANY:                          A N Y ;
AS:                           A S ;
ASC:                          A S C ;
AT:                           A T ;
AUTONOMOUS_TRANSACTION:       A U T O N O M O U S '_' T R A N S A C T I O N ;
BEGIN:                        B E G I N ;
BETWEEN:                      B E T W E E N ;
BIGINT :                      B I G I N T ;
BLOCK:                        B L O C K ;
BOOLEAN:                      B O O L E A N ;
BOTH:                         B O T H ;
BREADTH:                      B R E A D T H ;
BY:                           B Y ;
BYTE:                         B Y T E ;
CASCADE:                      C A S E C A D E ;
CASE:                         C A S E ;
CHARACTER:                    C H A R A C T E R ;
CHAR:                         C H A R ;
CHR:                          C H R ;
CLOSE:                        C L O S E ;
COMMIT:                       C O M M I T ;
CONNECT_BY_ROOT:              C O N N E C T '_' B Y '_' R O O T ;
CONNECT:                      C O N N E C T ;
CONSTANT:                     C O N S T A N T ;
CONTINUE:                     C O N T I N U E ;
COUNT:                        C O U N T ;
CREATE:                       C R E A T E;
CROSS:                        C R O S S ;
CUBE:                         C U B E ;
CURRENT:                      C U R R E N T ;
CURSOR:                       C U R S O R ;
DATE:                         D A T E ;
DATETIME:                     D A T E T I M E ;
DATETIMELTZ:                  D A T E T I M E L T Z ;
DATETIMETZ:                   D A T E T I M E T Z ;
DAY:                          D A Y ;
DEC:                          D E C ;
DECIMAL:                      D E C I M A L ;
DECLARE:                      D E C L A R E ;
DECREMENT:                    D E C R E M E N T ;
DEFAULT:                      D E F A U L T ;
DELETE:                       D E L E T E ;
DEPTH:                        D E P T H ;
DESC:                         D E S C ;
DISTINCT:                     D I S T I N C T ;
DIV:                          D I V ;
DOUBLE:                       D O U B L E ;
ELSE:                         E L S E ;
ELSIF:                        E L S I F ;
EMPTY:                        E M P T Y ;
END:                          E N D ;
ERROR:                        E R R O R ;
ESCAPE:                       E S C A P E ;
EXCEPTION:                    E X C E P T I O N ;
EXECUTE:                      E X E C U T E ;
EXISTS:                       E X I S T S ;
EXIT:                         E X I T ;
FALSE:                        F A L S E ;
FETCH:                        F E T C H ;
FIRST:                        F I R S T ;
FLOAT:                        F L O A T ;
FOLLOWING:                    F O L L O W I N G ;
FOR:                          F O R ;
FROM:                         F R O M ;
FULL:                         F U L L ;
FUNCTION:                     F U N C T I O N ;
GROUP:                        G R O U P ;
GROUPING:                     G R O U P I N G ;
HAVING:                       H A V I N G ;
HOUR:                         H O U R ;
IF:                           I F ;
IGNORE:                       I G N O R E ;
IMMEDIATE:                    I M M E D I A T E ;
INCREMENT:                    I N C R E M E N T ;
INFINITE:                     I N F I N I T E ;
IN:                           I N ;
INNER:                        I N N E R ;
INSERT:                       I N S E R T ;
INTEGER:                      I N T E G E R ;
INTERSECT:                    I N T E R S E C T ;
INTERVAL:                     I N T E R V A L ;
INT:                          I N T ;
INTO:                         I N T O ;
IS:                           I S ;
JOIN:                         J O I N ;
KEEP:                         K E E P ;
LAST:                         L A S T ;
LEADING:                      L E A D I N G ;
LEFT:                         L E F T ;
LIKE2:                        L I K E '2' ;
LIKE4:                        L I K E '4' ;
LIKEC:                        L I K E C ;
LIKE:                         L I K E ;
LIST:                         L I S T ;
LOCAL:                        L O C A L ;
LOCKED:                       L O C K E D ;
LONG:                         L O N G ;
LOOP:                         L O O P ;
MATCHED:                      M A T C H E D ;
MEMBER:                       M E M B E R ;
MERGE:                        M E R G E ;
MINUS:                        M I N U S ;
MINUTE:                       M I N U T E ;
MOD:                          M O D ;
MONTH:                        M O N T H ;
MULTISET:                     M U L T I S E T ;
NAN:                          N A N ;
NATURAL:                      N A T U R A L ;
NCHAR_CS:                     N C H A R '_' C S ;
NEW:                          N E W ;
NEXT:                         N E X T ;
NOCYCLE:                      N O C Y C L E ;
NOT:                          N O T ;
NOWAIT:                       N O W A I T ;
NULL_:                        N U L L ;
NULLS:                        N U L L S ;
NUMERIC:                      N U M E R I C ;
OFFSET:                       O F F S E T ;
OF:                           O F ;
ONLY:                         O N L Y ;
ON:                           O N ;
OPEN:                         O P E N ;
ORDER:                        O R D E R ;
OR:                           O R;
OUTER:                        O U T E R ;
OUT:                          O U T ;
OVERFLOW:                     O V E R F L O W ;
OVER:                         O V E R ;
PARTITION:                    P A R T I T I O N ;
PERCENT_FOUND:                '%' SPACE* F O U N D ;
PERCENT_ISOPEN:               '%' SPACE* I S O P E N;
PERCENT_NOTFOUND:             '%' SPACE* N O T F O U N D;
PERCENT_KEYWORD:              P E R C E N T ;
PERCENT_ROWCOUNT:             '%' SPACE* R O W C O U N T ;
PERCENT_ROWTYPE:              '%' SPACE* R O W T Y P E ;
PERCENT_TYPE:                 '%' SPACE* T Y P E ;
PRAGMA:                       P R A G M A ;
PRECEDING:                    P R E C E D I N G ;
PRECISION:                    P R E C I S I O N ;
PRESENT:                      P R E S E N T ;
PRIOR:                        P R I O R ;
PROCEDURE:                    P R O C E D U R E ;
RAISE:                        R A I S E ;
RAISE_APPLICATION_ERROR:      R A I S E '_' A P P L I C A T I O N '_' E R R O R ;
RANGE:                        R A N G E ;
RAW:                          R A W ;
REAL:                         R E A L ;
REF:                          R E F ;
REPLACE:                      R E P L A C E;
RESPECT:                      R E S P E C T ;
RETURN:                       R E T U R N ;
REVERSE:                      R E V E R S E ;
RIGHT:                        R I G H T ;
ROLLBACK:                     R O L L B A C K ;
ROLLUP:                       R O L L U P ;
ROW:                          R O W ;
ROWS:                         R O W S ;
SAMPLE:                       S A M P L E ;
SEARCH:                       S E A R C H ;
SECOND:                       S E C O N D ;
SEED:                         S E E D ;
SELECT:                       S E L E C T ;
SEQUENCE:                     S E Q U E N C E ;
SET:                          S E T ;
SETEQ:                        S E T E Q;
SETNEQ:                       S E T N E Q;
SETS:                         S E T S ;
SHORT:                        S H O R T ;
SIBLINGS:                     S I B L I N G S ;
SKIP_       :                 S K I P ;
SMALLINT:                     S M A L L I N T ;
SOME:                         S O M E ;
SQL:                          S Q L ;
START:                        S T A R T ;
STRING:                       S T R I N G ;
SUBMULTISET:                  S U B M U L T I S E T ;
SUBPARTITION:                 S U B P A R T I T I O N ;
SUBSET:                       S U B S E T ;
SUBSETEQ:                     S U B S E T E Q;
SUPERSET:                     S U P E R S E T;
SUPERSETEQ:                   S U P E R S E T E Q ;
SYS_REFCURSOR:                S Y S '_' R E F C U R S O R ;
TABLE:                        T A B L E ;
THEN:                         T H E N ;
THE:                          T H E ;
TIES:                         T I E S ;
TIMESTAMP:                    T I M E S T A M P ;
TIMESTAMPLTZ:                 T I M E S T A M P L T Z ;
TIMESTAMPTZ:                  T I M E S T A M P T Z ;
TIME:                         T I M E ;
TO:                           T O ;
TRAILING:                     T R A I L I N G ;
TRUE:                         T R U E ;
TRUNCATE:                     T R U N C A T E ;
TYPE:                         T Y P E ;
UNBOUNDED:                    U N B O U N D E D ;
UNION:                        U N I O N ;
UNIQUE:                       U N I Q U E ;
UPDATE:                       U P D A T E ;
USING:                        U S I N G ;
VALUES:                       V A L U E S ;
VALUE:                        V A L U E ;
VARCHAR:                      V A R C H A R ;
WAIT:                         W A I T ;
WHEN:                         W H E N ;
WHERE:                        W H E R E ;
WHILE:                        W H I L E ;
WITHIN:                       W I T H I N ;
WITH:                         W I T H ;
WORK:                         W O R K ;
XOR:                          X O R ;
YEAR:                         Y E A R ;
ZONE:                         Z O N E ;
CUME_DIST:                    C U M E '_' D I S T ;
DENSE_RANK:                   D E N S E '_' R A N K ;
LISTAGG:                      L I S T A G G ;
PERCENT_RANK:                 P E R C E N T '_' R A N K ;
PERCENTILE_CONT:              P E R C E N T I L E '_' C O N T ;
PERCENTILE_DISC:              P E R C E N T I L E '_' D I S C ;
RANK:                         R A N K ;
AVG:                          A V G ;
CORR:                         C O R R ;
COVAR_:                       C O V A R '_' ;
DECODE:                       D E C O D E ;
LAG:                          L A G ;
LEAD:                         L E A D ;
MAX:                          M A X ;
MEDIAN:                       M E D I A N ;
MIN:                          M I N ;
NTILE:                        N T I L E ;
NVL:                          N V L ;
RATIO_TO_REPORT:              R A T I O '_' T O '_' R E P O R T ;
REGR_:                        R E G R '_' ;
ROUND:                        R O U N D ;
ROW_NUMBER:                   R O W '_' N U M B E R ;
SUBSTR:                       S U B S T R ;
TO_CHAR:                      T O '_' C H A R ;
TRIM:                         T R I M ;
SUM:                          S U M ;
STDDEV:                       S T D D E V ;
VAR_:                         V A R '_' ;
VARIANCE:                     V A R I A N C E ;
VARYING:                      V A R Y I N G ;
LEAST:                        L E A S T ;
GREATEST:                     G R E A T E S T ;
TO_DATE:                      T O '_' D A T E ;

PERIOD2:  '..';
PERIOD:   '.';

FLOATING_POINT_NUM: BASIC_UINT? '.' [0-9]+ ([eE] ('+'|'-')? BASIC_UINT)? [fF]?;
UNSIGNED_INTEGER:    BASIC_UINT ([eE] ('+'|'-')? BASIC_UINT)?;

DELIMITED_ID: ('"' | '[' | '`') REGULAR_ID ('"' | ']' | '`') ;
CHAR_STRING: '\''  (~('\'' | '\r' | '\n') | '\'' '\'' | NEWLINE)* '\'';

NULL_SAFE_EQUALS_OP:          '<=>';

GE:             '>=';
LE:             '<=';
CONCAT_OP:      '||';
LT2:            '<<';
GT2:            '>>';
ASTERISK2:      '**';

LEFT_PAREN:                '(';
RIGHT_PAREN:               ')';
ASTERISK:                  '*';
PLUS_SIGN:                 '+';
MINUS_SIGN:                '-';
BIT_COMPLI:                '~';
COMMA:                     ',';
SOLIDUS:                   '/';
AT_SIGN:                   '@';
ASSIGN_OP:                 ':=';

NOT_EQUAL_OP:              '!='
            |              '<>'
            ;

AMPERSAND:          '&';
CARRET_OP:          '^';
EXCLAMATION_OP:     '!';
GT:                 '>';
LT:                 '<';
COLON:              ':';
SEMICOLON:          ';';

BAR:                '|';
EQUALS_OP:          '=';

LEFT_BRACKET:       '[';
RIGHT_BRACKET:      ']';

LEFT_BRACE:         '{';
RIGHT_BRACE:        '}';

INTRODUCER:         '_';

SINGLE_LINE_COMMENT:    '--' ~('\r' | '\n')* NEWLINE_EOF                 -> channel(HIDDEN);
SINGLE_LINE_COMMENT2:   '//' ~('\r' | '\n')* NEWLINE_EOF                 -> channel(HIDDEN);
MULTI_LINE_COMMENT:     '/*' .*? '*/'                                    -> channel(HIDDEN);

REGULAR_ID: SIMPLE_LETTER (SIMPLE_LETTER | '_' | [0-9])*;

SPACES: [ \t\r\n]+ -> channel(HIDDEN);

// Fragment rules

fragment BASIC_UINT     : '0'|[1-9][0-9]*;
fragment NEWLINE_EOF    : NEWLINE | EOF;
fragment SIMPLE_LETTER  : [A-Za-z] | [\uAC00-\uD7A3];   // English letters and Korean letters
fragment FLOAT_FRAGMENT : UNSIGNED_INTEGER* '.'? UNSIGNED_INTEGER+;
fragment NEWLINE        : '\r'? '\n';
fragment SPACE          : [ \t];

fragment A : [aA]; // match either an 'a' or 'A'
fragment B : [bB];
fragment C : [cC];
fragment D : [dD];
fragment E : [eE];
fragment F : [fF];
fragment G : [gG];
fragment H : [hH];
fragment I : [iI];
fragment J : [jJ];
fragment K : [kK];
fragment L : [lL];
fragment M : [mM];
fragment N : [nN];
fragment O : [oO];
fragment P : [pP];
fragment Q : [qQ];
fragment R : [rR];
fragment S : [sS];
fragment T : [tT];
fragment U : [uU];
fragment V : [vV];
fragment W : [wW];
fragment X : [xX];
fragment Y : [yY];
fragment Z : [zZ];
