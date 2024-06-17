package ru.nsu.testova.lab4.model.detail;

public class Car extends Detail {
    private Engine engine;
    private Body body;
    private Accessory accessory;
    public Car (Engine engine, Body body, Accessory accessory, String id) {
        super(DetailName.CAR, id);
        this.accessory = accessory;
        this.engine = engine;
        this.body = body;
    }

    public String getAccessoryId() {
        return accessory.getId();
    }
    public String getBodyId() {
        return body.getId();
    }
    public String getEngineId() {
        return engine.getId();
    }
}
