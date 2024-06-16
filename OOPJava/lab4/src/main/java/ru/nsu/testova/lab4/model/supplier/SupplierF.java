package ru.nsu.testova.lab4.model.supplier;

import ru.nsu.testova.lab4.model.ModelListener;
import ru.nsu.testova.lab4.model.RunningObjects;
import ru.nsu.testova.lab4.model.storage.Storage;

public class SupplierF implements Supplier, RunningObjects {
    protected ModelListener listener;
    protected Storage storage;
    protected int curObj = 0;
    protected long timeSleep;
    public SupplierF(ModelListener listener, Storage storage, long timeSleep ) {
        this.listener = listener;
        this.storage = storage;
        this.timeSleep = timeSleep;
    }
    @Override
    public void run() {

    }

    public void setTimeSleep(long time) {
        timeSleep = time;
    }
}
