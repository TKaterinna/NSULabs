package ru.nsu.testova;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Collections;
import java.util.SortedSet;
import java.util.TreeSet;

public class Parser {
    String fileName;
    int wordCount = 0;
    SortedSet<Word> table;
    Parser(String file) {
        fileName = file;
        table = new TreeSet<Word>(Collections.reverseOrder());
    }
//        try (BufferedReader rd = new BufferedReader(new FileReader(file))) {
//            HashMap<StringBuilder, Integer> table = new HashMap<StringBuilder, Integer>();
//            StringBuilder string = new StringBuilder();
//            char c;
//            while (rd.ready()) {
//                c = (char) rd.read();
//                if (Character.isLetterOrDigit(c)) {
//                    string.append(c);
//                }
//                else {
//                    //System.out.println(string);
//                    if (table.containsKey(string)) {
//                        table.replace(string, table.get(string) + 1);
//                    }
//                    else {
//                        table.put(string, 1);
//                    }
//                    string.setLength(0);
//                }
//            }
//            for (HashMap.Entry<StringBuilder, Integer> entry : table.entrySet()) {
//                StringBuilder key = entry.getKey();
//                Integer value = entry.getValue();
//                System.out.println(key + "," + value);
//            }
//
//
////            Map<StringBuilder, Integer> sortedMap = table.entrySet().stream()
////                    .sorted(Comparator.comparingInt(e -> e.getValue()))
////                    .collect(Collectors.toMap(
////                            Map.Entry::getKey,
////                            Map.Entry::getValue,
////                            (a, b) -> { throw new AssertionError(); },
////                            LinkedHashMap::new
////                    ));
////
////            sortedMap.entrySet().forEach(System.out::println);
//        } catch (IOException e) {
//            throw new RuntimeException(e);
//        }



//        try (InputStreamReader rd = new InputStreamReader(new FileInputStream(file))) {
//            StringBuilder string = new StringBuilder();
//            char c;
//            while (rd.ready()) {
//                c = rd.;
//                string.append(scanner.next());
//            }
//        } catch (IOException e) {
//            throw new RuntimeException(e);
//        }


//        try (Scanner scanner = new Scanner(new FileInputStream(file))) {
//            StringBuilder string = new StringBuilder();
//            char c;
//            while (scanner.hasNext() && Character.isLetterOrDigit()) {
//                c = scanner.next().charAt(0);
//                string.append(scanner.next());
//            }
//        } catch (FileNotFoundException e) {
//            throw new RuntimeException(e);
//        }

    public void read() throws IOException {
        BufferedReader rd = new BufferedReader(new FileReader(fileName));
        //SortedSet<Word> table = new TreeSet<Word>(Collections.reverseOrder());
        //HashSet<Word> table = new HashSet<Word>();
        StringBuilder string = new StringBuilder();
        char c;
        while (rd.ready()) {
            c = (char) rd.read();
            if (Character.isLetterOrDigit(c)) {
                string.append(c);
            }
            else if (!string.isEmpty()) {
                wordCount++;
                boolean flag = false;
                for (Word cur: table) {
                    if (cur.getWord().compareTo(string) == 0) {
                        table.add(new Word(new StringBuilder(string), cur.getCount() + 1));
                        table.remove(cur);
                        flag = true;
                        break;
                    }
                }
                if (!flag) {
                    table.add(new Word(new StringBuilder(string), 1));
                }
                string.setLength(0);
            }
        }
    }

    public int getWordCount() {
        return wordCount;
    }

    public SortedSet<Word> getTable() {
        return table;
    }
}
