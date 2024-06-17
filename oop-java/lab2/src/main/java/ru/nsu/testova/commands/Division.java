package ru.nsu.testova.commands;

import ru.nsu.testova.Command;
import ru.nsu.testova.Context;
import ru.nsu.testova.exeptions.IncorrectParams;

import java.util.List;
import java.util.Stack;

public class Division implements Command {
    public void make(List<String> params, Context context) throws IncorrectParams {
        Stack<Double> stack = context.getStack();
        double a1 = stack.pop();
        double a2 = stack.pop();
        if (a1 == 0) {
            throw new IncorrectParams("Division by zero");
        }
        stack.push(a2/a1);
    }
}
