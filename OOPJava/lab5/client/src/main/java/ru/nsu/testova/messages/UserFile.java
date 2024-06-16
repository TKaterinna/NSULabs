package ru.nsu.testova.messages;

public class UserFile implements IMessage {
    private TypeMessage type;
    private String nick;
    private int id;
    private String fileName;
    private int size;
    private String mimoType;
    public UserFile(String nick, int id, String fileName, int size, String mimoType) {
        this.nick = nick;
        this.id = id;
        this.fileName = fileName;
        this.size = size;
        this.mimoType = mimoType;
        this.type = TypeMessage.FILE;
    }
    @Override
    public TypeMessage getType() {
        return type;
    }

    @Override
    public String getMassage() {
        return nick + ": file " + fileName + "; id = " + id + "; size = " + size;
    }

    public int getId() {
        return id;
    }
}
