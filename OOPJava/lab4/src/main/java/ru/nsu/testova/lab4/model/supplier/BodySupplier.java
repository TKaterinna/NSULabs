package ru.nsu.testova.lab4.model.supplier;

import ru.nsu.testova.lab4.model.ModelListener;
import ru.nsu.testova.lab4.model.detail.Body;
import ru.nsu.testova.lab4.model.detail.Engine;
import ru.nsu.testova.lab4.model.storage.Storage;

public class BodySupplier extends SupplierF {
    public BodySupplier(ModelListener listener, Storage storage, long timeSleep) {
        super(listener, storage, timeSleep);
    }

    @Override
    public void run() {
        while (true) {
            try {
                Thread.sleep(timeSleep);
            } catch (InterruptedException e) {
                return;
            }
            storage.put(new Body("body" + "-" + curObj));
            //System.out.println("new body-" + curObj);
            curObj++;
        }
    }
}

