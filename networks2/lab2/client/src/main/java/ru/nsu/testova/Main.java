package ru.nsu.testova;

public class Main {

    public static void main(String[] args) {
        int port;
        String filePath;
        String ip;
        if (args.length == 3) {
            filePath = args[0];
            ip = args[1];
            port = Integer.parseInt(args[2]);
        }
        else {
            System.out.println("set path to file, ip, port");
            return;
        }
        Client client = new Client(filePath, ip, port);
        client.start();
    }
}