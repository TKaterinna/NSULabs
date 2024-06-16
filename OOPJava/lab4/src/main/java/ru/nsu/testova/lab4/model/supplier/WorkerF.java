package ru.nsu.testova.lab4.model.supplier;

import ru.nsu.testova.lab4.model.detail.Accessory;
import ru.nsu.testova.lab4.model.detail.Body;
import ru.nsu.testova.lab4.model.detail.Car;
import ru.nsu.testova.lab4.model.detail.Engine;
import ru.nsu.testova.lab4.model.storage.Storage;

public class WorkerF implements Supplier {
    private int curObj;
    private int numberWorker;
    private Storage engineStorage, carStorage, bodyStorage, accessoryStorage;
    public WorkerF(int numberWorker, Storage engineStorage, Storage carStorage, Storage bodyStorage, Storage accessoryStorage) {
        this.numberWorker = numberWorker;
        this.accessoryStorage = accessoryStorage;
        this.bodyStorage = bodyStorage;
        this.engineStorage = engineStorage;
        this.carStorage = carStorage;
        curObj = 0;
    }

    @Override
    public synchronized void run() {
        Engine engine = (Engine) engineStorage.get();
        if (engine == null) {
            return;
        }
        Body body = (Body) bodyStorage.get();
        if (body == null) {
            return;
        }
        Accessory accessory = (Accessory) accessoryStorage.get();
        if (accessory == null) {
            return;
        }

        try {
            Thread.sleep(500);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }

        Car car = new Car(engine, body, accessory, "car" + numberWorker + "-" + curObj);
        curObj++;
        carStorage.put(car);
    }
}
