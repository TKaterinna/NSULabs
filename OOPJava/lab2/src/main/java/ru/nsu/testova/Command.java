package ru.nsu.testova;

import ru.nsu.testova.exeptions.IncorrectParams;

import java.util.List;

public interface Command {
    public void make(List<String> params, Context context) throws IncorrectParams;
}


