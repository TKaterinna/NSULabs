package ru.nsu.testova.commands;

import ru.nsu.testova.Command;
import ru.nsu.testova.Context;

import java.util.List;
import java.util.Stack;

public class Print implements Command {
    public void make(List<String> params, Context context) {
        Stack<Double> stack = context.getStack();
        double a1 = stack.get(stack.size() - 1);
        System.out.println(a1);
    }
}
