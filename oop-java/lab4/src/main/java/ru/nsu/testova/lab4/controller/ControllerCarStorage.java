package ru.nsu.testova.lab4.controller;

import ru.nsu.testova.lab4.model.storage.Listener;
import ru.nsu.testova.lab4.model.storage.CarStorage;
import ru.nsu.testova.lab4.model.supplier.WorkerF;
import threadpool.MThreadPool;

import java.util.List;
import java.util.concurrent.*;

public class ControllerCarStorage implements ControllerStorage, Listener {
    private final CarStorage storage;
    private final ExecutorService serviceWork;
    private final List<WorkerF> workers;
    private final BlockingQueue<Runnable> queueW;
    private int currNum = 0;

    public ControllerCarStorage(CarStorage storage, List<WorkerF> workers) {
        this.storage = storage;
        this.workers = workers;
        queueW = new ArrayBlockingQueue<>(100);


        serviceWork = new MThreadPool(workers.size(), queueW);
//        serviceWork = new MExecutorService(workers.size(), queueW);


        //serviceWork = Executors.newCachedThreadPool();
        onModelChanged();
    }
    @Override
    public synchronized void onModelChanged() {
        int c = storage.getFreePlaceCount();
        if (c != 0) {
            serviceWork.submit(workers.get(currNum));
            currNum = (currNum + 1) % workers.size();
        }
//        for (WorkerF curr : workers) {
////            serviceWork.execute(curr);
//            serviceWork.submit(curr);
//            if (--c == 0) {
//                break;
//            }
//        }
    }

    public int getTaskCount() {
        return queueW.size();
    }

    public void stop() {
        serviceWork.shutdownNow();
    }
}
