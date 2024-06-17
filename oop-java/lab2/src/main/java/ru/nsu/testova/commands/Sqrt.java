package ru.nsu.testova.commands;

import ru.nsu.testova.Command;
import ru.nsu.testova.Context;
import ru.nsu.testova.exeptions.IncorrectParams;

import java.util.List;
import java.util.Stack;

public class Sqrt  implements Command {
    public void make(List<String> params, Context context) throws IncorrectParams {
        Stack<Double> stack = context.getStack();
        double a1 = stack.pop();
        if (a1 < 0) {
            throw new IncorrectParams("Sqrt of a negative number");
        }
        stack.push(Math.sqrt(a1));
    }
}
