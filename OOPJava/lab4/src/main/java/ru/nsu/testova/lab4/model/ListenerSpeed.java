package ru.nsu.testova.lab4.model;

import ru.nsu.testova.lab4.model.supplier.SupplierF;

public class ListenerSpeed {
    private RunningObjects obj;
    public ListenerSpeed(RunningObjects obj) {
        this.obj = obj;
    }
    public void onViewChanged(long newValue) {
        obj.setTimeSleep(newValue);
    }
}
