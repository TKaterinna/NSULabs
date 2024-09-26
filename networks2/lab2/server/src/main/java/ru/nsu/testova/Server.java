package ru.nsu.testova;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {
    private final int port;

    public Server(int port) {
        this.port = port;
    }

    public void start() {
        try (ServerSocket serverSocket = new ServerSocket(port)) {
            while (true) {
                Socket clientSocket = serverSocket.accept();
                Connection connection = new Connection(clientSocket);
                Thread t = new Thread(connection);

                t.start();
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
