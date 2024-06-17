package ru.nsu.testova.lab4.model.storage;

import ru.nsu.testova.lab4.model.ModelListener;
import ru.nsu.testova.lab4.model.detail.Detail;
import java.util.ArrayList;
import java.util.List;

public class StorageF implements Storage {
    protected ModelListener listener;
    protected final int maxSize;
    protected int numProducedObj;
    protected final List<Detail> obj;

    public StorageF(ModelListener listener, int maxSize) {
        this.listener = listener;
        this.maxSize = maxSize;
        obj = new ArrayList<>();
        numProducedObj = 0;
    }
    @Override
    public synchronized void put(Detail object) {
        while (obj.size() >= maxSize) {
            try {
                wait();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                //notifyAll();
                return;
            }
        }
        notifyAll();
        obj.add(object);
        numProducedObj++;
        //System.out.println(obj.size() + " " + maxSize);
        if (listener != null) {
            listener.onModelChanged();
        }
    }
    @Override
    public synchronized Detail get() {
        while (obj.isEmpty()) {
            try {
                wait();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                return null;
            }
        }
        notifyAll();
        if (listener != null) {
            listener.onModelChanged();
        }
        return obj.remove(obj.size() - 1);
    }
    public int getSize() {
        return obj.size();
    }

    public int getNumProducedObj() {
        return numProducedObj;
    }
}
