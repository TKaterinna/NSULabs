package ru.nsu.testova.lab4.model.supplier;

import ru.nsu.testova.lab4.model.ModelListener;
import ru.nsu.testova.lab4.model.detail.Accessory;
import ru.nsu.testova.lab4.model.detail.Body;
import ru.nsu.testova.lab4.model.storage.Storage;

public class AccessorySupplier extends SupplierF {
    private int numberSupplier;
    public AccessorySupplier(ModelListener listener, Storage storage, int numberSupplier, long timeSleep) {
        super(listener, storage, timeSleep);
        this.numberSupplier = numberSupplier;
    }

    @Override
    public void run() {
        while (true) {
            try {
                Thread.sleep(timeSleep);
            } catch (InterruptedException e) {
                return;
            }
            storage.put(new Accessory("accessory" + numberSupplier + "-" + curObj));
            //System.out.println("new accessory" + numberSupplier + "-" + curObj);
            curObj++;
        }
    }
}

