package ru.nsu.testova.messages;

public class UserLog implements IMessage {
    private TypeMessage type;
    private String message;
    public UserLog(String message) {
        type = TypeMessage.USERLOG;
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
