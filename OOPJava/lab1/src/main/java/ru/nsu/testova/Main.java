package ru.nsu.testova;

import java.io.IOException;

public class Main {
    public static void main(String[] args) {
        if (args.length == 0) {
            System.out.println("no input file");
        }
        else {
            try {
                Parser parser = new Parser(args[0]);
                parser.read();
                Csv csv = new Csv(parser.getTable(), parser.getWordCount());
            }
            catch (IOException | RuntimeException e) {
                System.out.println(e.getMessage());
            }
        }
    }
}