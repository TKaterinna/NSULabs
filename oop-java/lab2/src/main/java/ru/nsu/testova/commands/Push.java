package ru.nsu.testova.commands;

import ru.nsu.testova.Command;
import ru.nsu.testova.Context;
import ru.nsu.testova.exeptions.IncorrectParams;

import java.util.List;
import java.util.Map;
import java.util.Stack;

public class Push implements Command {
    public void make(List<String> params, Context context) throws IncorrectParams {
        Stack<Double> stack = context.getStack();
        try {
            double a1 = Double.parseDouble(params.get(1));
            stack.push(a1);
        } catch (NumberFormatException ex) {
            Map<String, Double> var = context.getVar();
            if (var.containsKey(params.get(1))) {
                stack.push(var.get(params.get(1)));
            }
            else {
                throw new IncorrectParams("No parameter \"" + params.get(1) + "\"");
            }
        }
    }
//    public void make(String[] params, Context context) throws IncorrectParams {
//        Stack<Double> stack = context.getStack();
//        try {
//            double a1 = Double.parseDouble(params[1]);
//            stack.push(a1);
//        } catch (NumberFormatException ex) {
//            Map<String, Double> var = context.getVar();
//            if (var.containsKey(params[1])) {
//                stack.push(var.get(params[1]));
//            }
//            else {
//                throw new IncorrectParams("No parameter \"" + params[1] + "\"");
//            }
//        }
//    }
}
