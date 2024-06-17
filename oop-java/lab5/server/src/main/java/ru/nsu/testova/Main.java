package ru.nsu.testova;

import java.io.*;
import java.util.Objects;

public class Main {
    private static int port;
    public static void main(String[] args) {
//        parseConfig();
        if (args.length == 1) {
            port = Integer.parseInt(args[0]);
        }
        Server server = new Server(port);
        server.start();
    }
    public static void parseConfig() {
        String configFile = "src/main/resources/ru/nsu/testova/config";
        try (InputStream is = new FileInputStream(configFile)) {
            try (BufferedReader file = new BufferedReader(new InputStreamReader(is))) {
                while (file.ready()) {
                    String[] str = file.readLine().split(" ");
                    if (Objects.equals(str[0], "port")) {
                        port = Integer.parseInt(str[1]);
                    }
                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}