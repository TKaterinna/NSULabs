package ru.nsu.testova;

public class Client {
    private String nick;
    private final int hashPassword;
    public Client(String nick, int hashPassword) {
        this.hashPassword = hashPassword;
        this.nick = nick;
    }

    public int getHashPassword() {
        return hashPassword;
    }

    public String getNick() {
        return nick;
    }
}
