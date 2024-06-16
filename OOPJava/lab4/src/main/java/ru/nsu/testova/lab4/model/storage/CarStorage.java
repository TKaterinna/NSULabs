package ru.nsu.testova.lab4.model.storage;

import ru.nsu.testova.lab4.controller.ControllerCarStorage;
import ru.nsu.testova.lab4.model.ModelListener;
import ru.nsu.testova.lab4.model.detail.Detail;

public class CarStorage extends StorageF{
    private ControllerCarStorage controllerCarStorage;
    private int taskCount = 0;
    public CarStorage(ModelListener listener, int maxSize) {
        super(listener, maxSize);
    }

    public synchronized void setListener(ControllerCarStorage controllerCarStorage) {
        this.controllerCarStorage = controllerCarStorage;
    }

    @Override
    public synchronized Detail get() {
        Detail detail = super.get();
        if (detail == null) {
            Thread.currentThread().interrupt();
        }
        taskCount = controllerCarStorage.getTaskCount();
        controllerCarStorage.onModelChanged();
        return detail;
    }

    public int getTaskCount() {
        return taskCount;
    }

    public synchronized int getFreePlaceCount() {
        return maxSize - obj.size();
    }
}
