package ru.nsu.testova.messages;

public class Message implements IMessage {
    private TypeMessage type;
    private String nick;
    private String message;
    public Message(String nick, String message) {
        type = TypeMessage.MESSAGE;
        this.nick = nick;
        this.message = message;
    }
    @Override
    public TypeMessage getType() {
        return type;
    }
    @Override
    public String getMassage() {
        return nick + ": " + message;
    }

    public String getNick() {
        return nick;
    }
}
