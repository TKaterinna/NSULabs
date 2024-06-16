package ru.nsu.testova;

import org.w3c.dom.Document;

import java.awt.*;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.*;
import java.util.List;

public class Server {
    private ServerSocket serverSocket;
    private List<String> lastMessages;
    private final int port;
    private List<Client> clientsBase;
    private List<Members> listMembers;
    private List<String> listFiles;
//    private int nextId = 0;
    private Map<String, String> avatars = null;
    private final Object lockCB = new Object();
    private final Object lockLMes = new Object();
    private final Object lockLMem = new Object();
    private final Object lockF = new Object();
    private final Object lockA = new Object();
    public Server(int port) {
        this.port = port;
        listMembers = new ArrayList<>();
        lastMessages = new ArrayList<>();
        clientsBase = new ArrayList<>();
        listFiles = new ArrayList<>();
        avatars = new HashMap<>();
        serverSocket = null;
    }
    public void start() {
        try {
            serverSocket = new ServerSocket(port);
            while (true) {
                Socket clientSocket = serverSocket.accept();
                Members newMember = new Members("", clientSocket);
                synchronized (lockLMem) {
                    listMembers.add(newMember);
                }
                Connection connection = new Connection(clientSocket, this, newMember);
                Thread t = new Thread(connection);
                t.start();
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        finally {
            if (serverSocket != null) {
                try {
                    serverSocket.close();
                } catch (IOException e) {
//                    throw new RuntimeException(e);
                }
            }
        }
    }

    public void sendAll(String string) {
        synchronized (lockLMem) {
            int n = listMembers.size();
            List<String> nicks = new ArrayList<>();
            for (int i = 0; i < n; ) {
                if (listMembers.get(i).isDisconnected()) {
                    nicks.add(listMembers.get(i).getNick());
                    listMembers.get(i).closeClient();
                    listMembers.remove(i);
                    n--;
                    continue;
                }
                if (listMembers.get(i).isLogin()) {
                    System.out.println("send to " + listMembers.get(i).getNick() + " sendAll");
                    listMembers.get(i).send(string);
//                i++;
                }
                i++;
            }
            for (String nick : nicks) {
                System.out.println(nick + " disconnected");
                sendAll("<event name=\"userlogout\"><name>" + nick + "</name></event>");
            }
        }
    }
    public void sendExcept(String string, Members expectForMember) {
        synchronized (lockLMem) {
            int n = listMembers.size();
            List<String> nicks = new ArrayList<>();
            for (int i = 0; i < n; ) {
                if (listMembers.get(i).isDisconnected()) {
                    nicks.add(listMembers.get(i).getNick());
                    listMembers.get(i).closeClient();
                    listMembers.remove(i);
                    n--;
                }
                if (listMembers.get(i).isLogin() && (listMembers.get(i) != expectForMember)) {
                    System.out.println("send to " + listMembers.get(i).getNick() + " sendExpect");
                    listMembers.get(i).send(string);
                    i++;
                } else {
                    i++;
                }
            }
            for (String nick : nicks) {
                System.out.println(nick + " disconnected");
                sendAll("<event name=\"userlogout\"><name>" + nick + "</name></event>");
            }
        }
    }

    public String findMemberCheckPassword(String nick, String password, Members currMember) {
        synchronized (lockCB) {
            int hashPassword = password.hashCode();
            for (Client c : clientsBase) {
                if (Objects.equals(c.getNick(), nick)) {
                    if (c.getHashPassword() == hashPassword) {
                        currMember.setNick(nick);
                        currMember.setLogin(true);
                        return null;
                    }
                    return "wrong password";
                }
            }
            if (!currMember.isLogin()) {
                currMember.setNick(nick);
                currMember.setLogin(true);
                clientsBase.add(new Client(nick, hashPassword));
                return null;
            }
            return "client is already login";
        }

//        int hashPassword = password.hashCode();
//        for (Members c : listMembers) {
//            if (Objects.equals(c.getNick(), nick)) {
//                return "nick is already used";
////                if (c.isLogin()) {
////                    return false;
////                }
////                c.setHashPassword(hashPassword);
////                c.setLogin(true);
////                return true;
////                return hashPassword == c.getHashPassword();
//            }
//        }
//        if (!currMember.isLogin()) {
//            currMember.setNick(nick);
//            currMember.setHashPassword(hashPassword);
//            currMember.setLogin(true);
//            return null;
//        }
//        return "client is already login";
    }
    public void removeMember(Members member) {
        synchronized (lockLMem) {
            listMembers.remove(member);
            System.out.println("after remove");
            for (Members m : listMembers) {
                System.out.println(m.getNick());
            }
        }
    }
    public List<String> getListMembers() {
        synchronized (lockLMem) {
            List<String> list = new ArrayList<>();
            for (Members c : listMembers) {
                if (!c.isDisconnected() && c.isLogin()) {
                    list.add(c.getNick());
                }
            }
            return list;
        }
    }

    public void addMessage(String message) {
        synchronized (lockLMes) {
            if (lastMessages.size() > 5) {
                lastMessages.remove(0);
            }
            lastMessages.add(message);
        }
    }

    public List<String> getLastMessages() {
        synchronized (lockLMes) {
            return new ArrayList<>(lastMessages);
        }
    }
    public int saveFile(String file) {
        synchronized (lockF) {
//            return nextId++;
            listFiles.add(file);
            return listFiles.size();
        }
    }
    public String getFile(int id) {
        synchronized (lockF) {
            return listFiles.get(id - 1);
        }
    }
    public void addAvatar(String nick, String message) {
        synchronized (lockA) {
            if (avatars.containsKey(nick)) {
                avatars.replace(nick, message);
            } else {
                avatars.put(nick, message);
            }
        }
    }

    public Map<String, String> getAvatars() {
        synchronized (lockA) {
            return new HashMap<>(avatars);
        }
    }
}
