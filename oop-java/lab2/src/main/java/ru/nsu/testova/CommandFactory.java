package ru.nsu.testova;

import ru.nsu.testova.exeptions.CalcExceptions;
import ru.nsu.testova.exeptions.SystemExceptions;

public interface CommandFactory {
    public Command create(String command) throws CalcExceptions, SystemExceptions;
}
