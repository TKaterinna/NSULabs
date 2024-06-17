package ru.nsu.testova.lab4.model.supplier;

import ru.nsu.testova.lab4.model.ModelListener;
import ru.nsu.testova.lab4.model.detail.Engine;
import ru.nsu.testova.lab4.model.storage.Storage;

public class EngineSupplier extends SupplierF {
    public EngineSupplier(ModelListener listener, Storage storage, long timeSleep) {
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
            storage.put(new Engine("engine" + "-" + curObj));
            //System.out.println("new engine-" + curObj);
            curObj++;
        }
    }
}
