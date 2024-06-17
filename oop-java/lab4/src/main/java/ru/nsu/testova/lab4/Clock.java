package ru.nsu.testova.lab4;

public class Clock {
    private long start;

    public Clock() {
        start = System.currentTimeMillis();
    }

    public long getTime() {
        return System.currentTimeMillis() - start;
    }
}
