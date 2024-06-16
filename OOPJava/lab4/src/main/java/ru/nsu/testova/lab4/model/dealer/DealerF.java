package ru.nsu.testova.lab4.model.dealer;

import ru.nsu.testova.lab4.Clock;
import ru.nsu.testova.lab4.model.ModelListener;
import ru.nsu.testova.lab4.model.RunningObjects;
import ru.nsu.testova.lab4.model.detail.Car;
import ru.nsu.testova.lab4.model.storage.StorageF;

public class DealerF implements Runnable, RunningObjects {
    private StorageF storage;
    private int numberDealers;
    private long timeSleep;
    private Clock clock;

    public DealerF(StorageF storage, int numberDealers, int timeSleep, Clock clock) {
        this.storage = storage;
        this.numberDealers = numberDealers;
        this.timeSleep = timeSleep;
        this.clock = clock;
    }

    public void get() {
        while (true) {
            try {
                Thread.sleep(timeSleep);
            } catch (InterruptedException e) {
                return;
            }
            Car obj = (Car)storage.get();
            if (obj == null) {
                return;
            }
            System.out.println(clock.getTime() + ": Dealer " + numberDealers + ": Auto " + obj.getId() +
                    " (Body: " + obj.getBodyId() + ", Engine: " + obj.getEngineId() + ", Accessory: " + obj.getAccessoryId() +")");
            //<Time>: Dealer <Number>: Auto <ID> (Body: <ID>, Motor: <ID>, Accessory: <ID>)
        }

    }
    @Override
    public void run() {
        get();
    }
    public void setTimeSleep(long time) {
        timeSleep = time;
    }
}