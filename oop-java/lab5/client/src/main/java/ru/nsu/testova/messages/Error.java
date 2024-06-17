package ru.nsu.testova.messages;

public class Error implements IMessage {
    private TypeMessage type;
    private String message;
    public Error(String message) {
        type = TypeMessage.ERROR;
        this.message = message;
    }
    @Override
    public TypeMessage getType() {
        return type;
    }
    @Override
    public String getMassage() {
        return message;
    }
}
