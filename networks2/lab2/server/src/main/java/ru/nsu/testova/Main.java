package ru.nsu.testova;

import java.io.File;

public class Main {
    public static void deleteDirectory(File directory) {
        File[] contents = directory.listFiles();
        if (contents != null) {
            for (File file : contents) {
                deleteDirectory(file);
            }
        }
        if (!directory.delete()) {
            System.out.println("Не удалось удалить директорию ./uploads");
        }
    }

    public static void main(String[] args) {
        Runtime.getRuntime().addShutdownHook(new Thread() {
            public void run() {
                File directory = new File("uploads");
                deleteDirectory(directory);
                System.out.println("Директория ./uploads удалена");
            }
        });
        int port;
        if (args.length == 1) {
            port = Integer.parseInt(args[0]);
        }
        else {
            System.out.println("set port");
            return;
        }
        Server server = new Server(port);
        server.start();
    }
}