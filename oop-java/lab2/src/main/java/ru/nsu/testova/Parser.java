package ru.nsu.testova;

import java.util.List;

public class Parser {
    Parser() {}
    public boolean parse(String line, List<String> params){
        if (line.isEmpty()) {
            return false;
        }
        if (line.charAt(0) == '#') {
            params.add("#");
            params.add(line.replaceFirst("#", ""));
        }
        else {
            String[] p = line.split(" ");
            for (String str : p) {
                if (!str.isEmpty()) {
                    params.add(str);
                }
            }
        }
        return true;
    }
}
