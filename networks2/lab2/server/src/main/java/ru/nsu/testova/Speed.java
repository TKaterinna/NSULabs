//package ru.nsu.testova;
//
//public class Speed implements Runnable {
//    private Connection connection = null;
//    private long pid = 0;
//    private int timeout = 0;
//
//    public Speed(Connection connection, int timeout, long pid) {
//        this.connection = connection;
//        this.timeout = timeout;
//        this.pid = pid;
//    }
//
//    @Override
//    public void run() {
//        System.out.println(pid + ": " + connection.getBytesCount()/timeout);
//        connection.setBytesCount(0);
//    }
//}
