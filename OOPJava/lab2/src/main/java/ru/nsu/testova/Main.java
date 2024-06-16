package ru.nsu.testova;

import ru.nsu.testova.exeptions.CalcExceptions;
import ru.nsu.testova.exeptions.SystemExceptions;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Main {
    public static void main(String[] args) {
        try {
            try(BufferedReader rd = (args.length == 0) ?
                    new BufferedReader(new InputStreamReader(System.in)) : new BufferedReader(new FileReader(args[0]))) {
                Calculator calc = new Calculator();

                CommandFactory factory = (args.length == 2) ? new CommandFactoryByConfig(args[1]) : new CommandFactoryByConfig();
                ProxyFactory proxyFactory = new ProxyFactory(factory);
                calc.calculate(rd, proxyFactory);
            }
        }
        catch (CalcExceptions ex) {
            System.out.println(ex.getMessage());
        }
        catch (SystemExceptions | IOException ex) {
            System.out.println("System errors\n");
            System.out.println(ex.getMessage());
        }
    }
}