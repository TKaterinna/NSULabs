package ru.nsu.testova.lab4.model.detail;

public class Detail {
    private DetailName name;
    private String id;

    public Detail(DetailName name, String id) {
        this.name = name;
        this.id = id;
    }

    public DetailName getName() {
        return name;
    }
    public String getId() {
        return id;
    }
}
