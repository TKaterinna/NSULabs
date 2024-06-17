package ru.nsu.testova;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;

public class Members {
    private String nick;
    private Socket socket;
//    private PrintWriter out = null;
    private DataOutputStream out = null;
    private boolean isLogin = false;
//    private BufferedReader in = null;
    public Members(String nick, Socket socket) {
        this.nick = nick;
        this.socket = socket;
        try {
//            out = new PrintWriter(socket.getOutputStream(), true);
            out = new DataOutputStream(socket.getOutputStream());
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public boolean isDisconnected() {
        return (socket.isClosed());
    }
    public void send(String string) {
        if (out != null) {
            try {
//                out.write(ByteBuffer.allocate(4).putInt(string.getBytes().length).array());
                out.writeInt(string.getBytes().length);
                out.write(string.getBytes(), 0, string.getBytes().length);
            } catch (SocketException e) {
                try {
                    socket.close();
                } catch (IOException ex) {
                    throw new RuntimeException(ex);
                }
            }
            catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
//        if (out != null) {
//            out.println(string);
//        }
    }
    public void setNick(String nick) {
        this.nick = nick;
    }
    public String getNick() {
        return nick;
    }
    public boolean isLogin() {
        return isLogin;
    }
    public void setLogin(boolean login) {
        isLogin = login;
    }
    public void closeClient() {
        try {
            socket.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        try {
            out.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
