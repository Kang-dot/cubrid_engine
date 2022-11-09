package com.cubrid.plcsql.driver;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

import com.cubrid.plcsql.compiler.antlrgen.PcsLexer;
import com.cubrid.plcsql.compiler.antlrgen.PcsParser;

import com.cubrid.plcsql.compiler.ParseTreePrinter;
import com.cubrid.plcsql.compiler.ParseTreeConverter;

import com.cubrid.plcsql.compiler.ast.Unit;

import java.io.File;
import java.io.FileInputStream;
import java.io.PrintStream;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.IOException;

import java.util.Map;
import java.util.TreeMap;

public class TestDriver
{
    private static ParseTree parse(String inFilePath) {

        long t0, t;

        t0 = System.currentTimeMillis();

        File f = new File(inFilePath);
        if (!f.isFile()) {
            throw new RuntimeException(inFilePath + " is not a file");
        }

        System.out.println(String.format("  creating File: %f sec",
            ((t = System.currentTimeMillis()) - t0) / 1000.0));
        t0 = t;

        FileInputStream in;
        try {
            in = new FileInputStream(f);
        } catch (FileNotFoundException e) {
            throw new RuntimeException(e);
        }

        System.out.println(String.format("  creating FileInputStream: %f sec",
            ((t = System.currentTimeMillis()) - t0) / 1000.0));
        t0 = t;

        ANTLRInputStream input;
        try {
            input = new ANTLRInputStream(in);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        System.out.println(String.format("  creating ANTLRInputStream: %f sec",
            ((t = System.currentTimeMillis()) - t0) / 1000.0));
        t0 = t;

        PcsLexer lexer = new PcsLexer(input);
        CommonTokenStream tokens = new CommonTokenStream(lexer);
        PcsParser parser = new PcsParser(tokens);

        SyntaxErrorIndicator sei = new SyntaxErrorIndicator();
        parser.addErrorListener(sei);

        System.out.println(String.format("  creating PcsLexer: %f sec",
            ((t = System.currentTimeMillis()) - t0) / 1000.0));
        t0 = t;

        ParseTree ret = parser.sql_script();

        System.out.println(String.format("  calling parser: %f sec",
            (System.currentTimeMillis() - t0) / 1000.0));

        if (sei.hasError) {
            throw new RuntimeException("syntax error");
        }

        return ret;
    }

    private static PrintStream getParseTreePrinterOutStream(int seq) {

        // create a output stream to print parse tree
        String outfile = String.format("./pt/T%05d.pt", seq);
        File g = new File(outfile);
        try {
            return new PrintStream(g);
        } catch (FileNotFoundException e) {
            throw new RuntimeException(e);
        }
    }

    private static PrintStream getJavaCodeOutStream(String className) {

        String outfile = String.format("./pt/%s.java", className);
        File g = new File(outfile);
        if (g.exists()) {
            throw new RuntimeException("file exists: " + outfile);
        }

        try {
            return new PrintStream(g);
        } catch (FileNotFoundException e) {
            throw new RuntimeException(e);
        }
    }

    public static void main(String[] args) {

        if (args.length == 0) {
           throw new RuntimeException("requires arguments (PL/CSQL file paths)");
        }

        long t, t0;

        for (int i = 0; i < args.length; i++) {

            System.out.println(String.format("file #%d: %s", i, args[i]));

            try {
                t0 = System.currentTimeMillis();

                String infile = args[i];
                ParseTree tree = parse(infile);
                if (tree == null) {
                    throw new RuntimeException("parsing failed");
                }

                System.out.println(String.format("parsing: %f sec", ((t = System.currentTimeMillis()) - t0) / 1000.0));
                t0 = t;

                // walk with a pretty printer to print parse tree
                PrintStream out = getParseTreePrinterOutStream(i);
                ParseTreePrinter pp = new ParseTreePrinter(out, infile);
                ParseTreeWalker.DEFAULT.walk(pp, tree);
                out.close();

                System.out.println(String.format("printing: %f sec", ((t = System.currentTimeMillis()) - t0) / 1000.0));
                t0 = t;

                ParseTreeConverter converter = new ParseTreeConverter();
                Unit unit = (Unit) converter.visit(tree);
                out = getJavaCodeOutStream(unit.getClassName());
                out.println(String.format("// seq=%05d, input-file=%s", i, infile));
                out.print(unit.toJavaCode());
                out.close();

                System.out.println(String.format("converting: %f sec", ((t = System.currentTimeMillis()) - t0) / 1000.0));
                t0 = t;

                System.out.println(" - success");
            } catch (Throwable e) {
                System.out.println(" - failure");
                throw e;
            }
        }

    }

    private static class SyntaxErrorIndicator extends BaseErrorListener {

        boolean hasError = false;

        @Override
        public void syntaxError(
                Recognizer<?,?> recognizer,
                Object offendingSymbol,
                int line,
                int charPositionInLine,
                String msg,
                RecognitionException e
            ) {
                hasError = true;
        }
    }
}
