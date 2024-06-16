package ru.nsu.testova.commands;

import ru.nsu.testova.Command;
import ru.nsu.testova.Context;

import java.util.List;
import java.util.Stack;

public class Multiplication implements Command {
    public void make(List<String> params, Context context) {
        Stack<Double> stack = context.getStack();
        double a1 = stack.pop();
        double a2 = stack.pop();
        stack.push(a2*a1);
    }
}
