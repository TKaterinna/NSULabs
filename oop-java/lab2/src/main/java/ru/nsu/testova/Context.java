package ru.nsu.testova;

import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

public class Context {
    private final Stack<Double> stack = new Stack<Double>();
    private final Map<String, Double> var = new HashMap<String, Double>();

    public Map<String, Double> getVar() {
        return var;
    }

    public Stack<Double> getStack() {
        return stack;
    }
}
