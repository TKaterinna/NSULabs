package ru.nsu.testova.commands;

import ru.nsu.testova.Command;
import ru.nsu.testova.Context;
import ru.nsu.testova.exeptions.IncorrectParams;

import java.util.List;
import java.util.Map;

public class Define implements Command {
    public void make(List<String> params, Context context) throws IncorrectParams {
        Map<String, Double> var = context.getVar();
        try {
            double params2 = Double.parseDouble(params.get(2));
            if (var.containsKey(params.get(1))) {
                var.replace(params.get(1), params2);
            }
            else {
                var.put(params.get(1), params2);
            }
        }
        catch (NumberFormatException ex) {
            throw new IncorrectParams(params.get(2) + " not a number\n");
        }
    }
}
