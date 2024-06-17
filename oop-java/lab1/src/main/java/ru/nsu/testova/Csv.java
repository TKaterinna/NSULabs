package ru.nsu.testova;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.SortedSet;

public class Csv {
    Csv(SortedSet<Word> table, double wordCount) throws IOException {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter("out.csv", false))) {
            writer.write("Слово,Частота,Частота(%)\n");
            for (Word cur: table) {
                writer.write(cur.getWord() + "," + cur.getCount() + "," + cur.getCount()/wordCount * 100 + "%\n");
            }
        }
    }
}
