package ru.nsu.testova.exeptions;

public class IncorrectParams extends CalcExceptions {
    public IncorrectParams(String message) {
        super("Incorrect parameters: " + message);
    }
}