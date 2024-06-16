package ru.nsu.testova.lab4.model.storage;

import ru.nsu.testova.lab4.model.detail.Detail;

import java.util.ArrayList;
import java.util.List;

public interface Storage {
    void put(Detail object);
    Detail get();
}
