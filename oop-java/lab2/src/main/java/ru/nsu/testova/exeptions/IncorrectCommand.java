package ru.nsu.testova.exeptions;

public class IncorrectCommand extends CalcExceptions{
    public IncorrectCommand(String message) {
        super("Unknown command: " + message);
    }
}
