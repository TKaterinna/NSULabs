package ru.nsu.testova;

import ru.nsu.testova.exeptions.CalcExceptions;
import ru.nsu.testova.exeptions.SystemExceptions;


import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Calculator {
    private final Context context = new Context();
    Calculator() {};
    public void calculate(BufferedReader rd, CommandFactory factory) throws IOException, CalcExceptions, SystemExceptions {
        String line;
        final Parser parser = new Parser();
        while ((line = rd.readLine())!= null) {
            List<String> params = new ArrayList<>();
            if (!parser.parse(line, params)) {
                continue;
            }

            Command command = factory.create(params.get(0));
            command.make(params, context);
        }


    }
}
