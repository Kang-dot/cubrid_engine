/*
 * Copyright (c) 2016 CUBRID Corporation.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the <ORGANIZATION> nor the names of its contributors
 *   may be used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

package com.cubrid.plcsql.compiler.ast;

import com.cubrid.plcsql.compiler.Misc;

public class ExprGlobalFuncCall implements Expr {

    public final int level;
    public final String name;
    public final NodeList<Expr> args;

    public ExprGlobalFuncCall(int level, String name, NodeList<Expr> args) {
        this.level = level;
        this.name = name;
        this.args = args;
    }

    @Override
    public String toJavaCode() {

        int argSize = args.nodes.size();
        String dynSql = getDynSql(name, argSize);
        String paramStr = getParametersStr(argSize);
        String setUsedValuesStr = getSetUsedValuesStr(argSize);

        return tmplStmt.replace("%FUNC-NAME%", name)
                .replace("%DYNAMIC-SQL%", dynSql)
                .replace("%PARAMETERS%", paramStr)
                .replace("    %SET-USED-VALUES%", Misc.indentLines(setUsedValuesStr, 2))
                .replace("  %ARGUMENTS%", Misc.indentLines(args.toJavaCode(",\n"), 1))
                .replace("%LEVEL%", "" + level);
    }

    // --------------------------------------------------
    // Private
    // --------------------------------------------------

    private static final String tmplStmt =
            Misc.combineLines(
                    "(new Object() { // global function call: %FUNC-NAME%",
                    "  Object invoke(%PARAMETERS%) throws Exception {",
                    "    String dynSql_%LEVEL% = \"%DYNAMIC-SQL%\";",
                    "    CallableStatement stmt_%LEVEL% = conn.prepareCall(dynSql_%LEVEL%);",
                    "    stmt_%LEVEL%.registerOutParameter(1, java.sql.Types.OTHER);",
                    "    %SET-USED-VALUES%",
                    "    stmt_%LEVEL%.execute();",
                    "    Object ret_%LEVEL% = stmt_%LEVEL%.getObject(1);",
                    "    stmt_%LEVEL%.close();",
                    "    return ret_%LEVEL%;",
                    "  }",
                    "}.invoke(",
                    "  %ARGUMENTS%",
                    "))");

    private static String getDynSql(String name, int argCount) {
        return String.format("?= call %s(%s)", name, Common.getQuestionMarks(argCount));
    }

    private static String getParametersStr(int size) {

        StringBuffer sbuf = new StringBuffer();
        boolean first = true;
        for (int i = 0; i < size; i++) {
            if (first) {
                first = false;
            } else {
                sbuf.append(", ");
            }

            sbuf.append("Object o" + i);
        }

        return sbuf.toString();
    }

    private static String getSetUsedValuesStr(int size) {

        StringBuffer sbuf = new StringBuffer();
        boolean first = true;
        for (int i = 0; i < size; i++) {
            if (first) {
                first = false;
            } else {
                sbuf.append(";\n");
            }

            sbuf.append(String.format("stmt_%%LEVEL%%.setObject(%d, o%d);", i + 2, i));
        }

        return sbuf.toString();
    }
}
