package ru.nsu.testova.client;

import javafx.scene.control.skin.SliderSkin;
import javafx.scene.image.Image;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;
import ru.nsu.testova.client.Client;
import ru.nsu.testova.messages.*;
import ru.nsu.testova.messages.Error;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import java.io.*;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class ReadThread implements Runnable {
    private Socket socket = null;
    private Client client = null;
    private int id = 0;

    public ReadThread(Socket socket, Client client) {
        this.socket = socket;
        this.client = client;
    }
    @Override
    public void run() {
        try (DataInputStream in = new DataInputStream(socket.getInputStream())) {
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            DocumentBuilder builder = null;
            Document document = null;
            try {
                builder = factory.newDocumentBuilder();
            } catch (ParserConfigurationException e) {
                throw new RuntimeException(e);
            }
            String message = null, type, nick;
            byte[] messageSize = new byte[4];
            byte[] messageB = new byte[78000000];
            int res, lenMes, temp = 0;
            while (!socket.isClosed()) {
                try {
//                    res = in.read(messageSize, 0, 4);
                    lenMes = in.readInt();
                    if (lenMes == -1) {
                        continue;
                    }
//                    lenMes = ByteBuffer.wrap(messageSize).getInt();
                    if (lenMes > messageB.length) {
                        client.setErrorFlag(true);
                        continue;
                    }
                    res = 0;
                    while (res != lenMes) {
                        temp = in.read(messageB, res, lenMes - res);
                        if (temp == -1) {
                            break;
                        }
                        res += temp;
                    }
                    if (temp == -1) {
                        client.setErrorFlag(true);
                        System.out.println("incorrect len of message");
                        continue;
                    }

                    System.out.println(new String(messageB, 0, res, StandardCharsets.UTF_8));

                    document = builder.parse(new ByteArrayInputStream(messageB, 0 , lenMes));
                    type = document.getDocumentElement().getTagName();
                    if (Objects.equals(type, "error")) {
                        try {
                            client.setErrorFlag(true);
                            if (getNodeByName(document, "message") != null) {
                                message = getTextContentByName(document, "message");
                            }

//                            Node node = document.getElementsByTagName("message").item(0);
//                            if (node != null) {
//                                message = node.getTextContent();
//                            }
                            else {
                                message = "error";
                            }
                            client.addErrorsList(new Error(message));
                        }
                        catch (Exception e) {
                            System.out.println("Incorrect error message");
                            System.out.println(e.getMessage());
                            continue;
                        }
                    }
                    else if (Objects.equals(type, "success")) {
                        try {
                            client.setSuccessFlag(true);
                            NodeList nodeList = document.getDocumentElement().getElementsByTagName("users");
                            if (nodeList.getLength() != 0) {
                                nodeList = document.getDocumentElement().getElementsByTagName("name");
                                List<String> listM = new ArrayList<>();
                                Node node;
                                for (int i = 0; i < nodeList.getLength(); i++) {
                                    node = nodeList.item(i);
                                    if (node != null) {
                                        listM.add(node.getTextContent());
                                    }
                                    else {
                                        listM.add("*no name*");
                                    }
                                }
                                client.addMessageList(new ListMembers(listM));
                            }
                            else {
                                if (getNodeByName(document, "id") != null) {
//                                Node node = document.getDocumentElement().getElementsByTagName("id").item(0);
//                                if (node != null) {
                                    if (getNodeByName(document, "content") != null) {
//                                    node = document.getDocumentElement().getElementsByTagName("content").item(0);
//                                    if (node != null) {
                                        byte[] decodedBytes = Base64.getDecoder().decode(getTextContentByName(document, "content"));
                                        if (getNodeByName(document, "encoding") == null || !getTextContentByName(document, "encoding").equals("base64")) {
                                            System.out.println("supported only base64");
                                            continue;
                                        }

//                                        byte[] decodedBytes = Base64.getDecoder().decode(node.getTextContent());
//                                        node = document.getDocumentElement().getElementsByTagName("encoding").item(0);
                                        File dir = client.getDir();
//                                        if (node == null || !node.getTextContent().equals("base64")) {
//                                            System.out.println("supported only base64");
//                                            continue;
//                                        }
//                                        byte[] decodedBytes = Base64.getDecoder().decode(document.getElementsByTagName("success").item(0).getChildNodes().item(4).getChildNodes().item(0).getNodeValue());
                                        String id = "*no id*", fileName = "*no file name*", mimeType = "*no type*";
                                        if (getNodeByName(document, "mimeType") != null) {
                                            mimeType = getTextContentByName(document, "mimeType");
                                        }
                                        if (getNodeByName(document, "name") != null) {
                                            fileName = getTextContentByName(document, "name");
                                        }
                                        if (getNodeByName(document, "id") != null) {
                                            id = getTextContentByName(document, "id");
                                        }
//                                        node = document.getDocumentElement().getElementsByTagName("mimeType").item(0);
//                                        if (node != null) {
//                                            mimeType = node.getTextContent();
//                                        }
//                                        node = document.getDocumentElement().getElementsByTagName("name").item(0);
//                                        if (node != null) {
//                                            fileName = node.getTextContent();
//                                        }
//                                        node = document.getDocumentElement().getElementsByTagName("id").item(0);
//                                        if (node != null) {
//                                            id = node.getTextContent();
//                                        }
                                        FileOutputStream fos = new FileOutputStream(dir.getPath() + "/" + id + "-" + fileName);
                                        fos.write(decodedBytes);
                                        fos.flush();
                                        fos.close();
                                    }
                                    else {
                                        message = "file " + getTextContentByName(document, "id");
//                                        node = document.getDocumentElement().getElementsByTagName("id").item(0);
//                                        message = "file " + node.getTextContent();
                                        client.addMessageList(new Message("Me", message));
                                    }
                                }
                                else {
                                    message = "success";
                                }
                            }
                        }
                        catch (Exception e) {
                            System.out.println("Incorrect success message");
                            System.out.println(e.getMessage());
                            continue;
                        }
                    }
                    else if (Objects.equals(type, "event")) {
                        try {
                            String eventType = document.getDocumentElement().getAttribute("name");
                            switch (eventType) {
                                case "message" -> {
                                    if (getNodeByName(document, "message") != null) {
                                        message = getTextContentByName(document, "message");
                                    }
//                                    Node node = document.getElementsByTagName("message").item(0);
//                                    if (node != null) {
//                                        message = node.getTextContent();
//                                    }
                                    else {
                                        message = "*null*";
                                    }
                                    if (getNodeByName(document, "from") != null) {
                                        nick = getTextContentByName(document, "from");
                                    }
//                                    node = document.getElementsByTagName("from").item(0);
//                                    if (node != null) {
//                                        nick = node.getTextContent();
//                                    }
                                    else {
                                        nick = "*no name*";
                                    }
                                    client.addMessageList(new Message(nick, message));
                                }
                                case "userlogin" -> {
                                    if (getNodeByName(document, "name") != null) {
                                        message = getTextContentByName(document, "name");
                                    }
//                                    Node node = document.getElementsByTagName("name").item(0);
//                                    if (node != null) {
//                                        message = node.getTextContent();
//                                    }
                                    else {
                                        message = "*no name*";
                                    }
                                    client.addMessageList(new UserLog(message + "login"));
                                }
                                case "userlogout" -> {
                                    if (getNodeByName(document, "name") != null) {
                                        message = getTextContentByName(document, "name");
                                    }
//                                    Node node = document.getElementsByTagName("name").item(0);
//                                    if (node != null) {
//                                        message = node.getTextContent();
//                                    }
                                    else {
                                        message = "*no name*";
                                    }
                                    client.addMessageList(new UserLog(message + "logout"));
                                }
                                case "file" -> {
                                    int fId = -1;
                                    if (getNodeByName(document, "id") != null) {
                                        fId = Integer.parseInt(getTextContentByName(document, "id"));
                                    }
//                                    Node node = document.getElementsByTagName("id").item(0);
//                                    if (node != null) {
//                                        fId = Integer.parseInt(node.getTextContent());
//                                    }
                                    String fNick = "*no name*", fName = "*no file name*", fType = "*no file type*";
                                    int fSize = -1;
                                    if (getNodeByName(document, "from") != null) {
                                        fNick = getTextContentByName(document, "from");
                                    }
                                    if (getNodeByName(document, "name") != null) {
                                        fName = getTextContentByName(document, "name");
                                    }
                                    if (getNodeByName(document, "mimeType") != null) {
                                        fType = getTextContentByName(document, "mimeType");
                                    }
                                    if (getNodeByName(document, "size") != null) {
                                        fSize = Integer.parseInt(getTextContentByName(document, "size"));
                                    }
//                                    node = document.getElementsByTagName("from").item(0);
//                                    if (node != null) {
//                                        fNick = node.getTextContent();
//                                    }
//                                    node = document.getElementsByTagName("name").item(0);
//                                    if (node != null) {
//                                        fName = node.getTextContent();
//                                    }
//                                    int fSize = -1;
//                                    node = document.getElementsByTagName("size").item(0);
//                                    if (node != null) {
//                                        fSize = Integer.parseInt(node.getTextContent());
//                                    }
//                                    node = document.getElementsByTagName("mimeType").item(0);
//                                    if (node != null) {
//                                        fType = node.getTextContent();
//                                    }

                                    message = fNick + ": file " + fName + "; id = " + fId + "; size = " + fSize;
                                    client.addMessageList(new UserFile(fNick, fId, fName, fSize, fType));
                                }
                                case "avatar" -> {
//                                    "<event name=\"avatar\"><from>" + currMember.getNick() + "</from><mimeType>" + mimeType + "</mimeType><encoding>base64</encoding><content>" + base64Text + "</content></event>
                                    String fNick = "*no name*";
                                    if (getNodeByName(document, "from") != null) {
                                        fNick = getTextContentByName(document, "from");
                                    }
                                    if (getNodeByName(document, "encoding") == null || !getTextContentByName(document, "encoding").equals("base64")) {
                                        System.out.println("supported only base64");
                                        continue;
                                    }
                                    if (getNodeByName(document, "content") == null) {
                                        System.out.println("Null avatar file");
                                        message = null;
                                    }
//                                    Node node = document.getElementsByTagName("from").item(0);
//                                    if (node != null) {
//                                        fNick = node.getTextContent();
//                                    }
//                                    node = document.getElementsByTagName("encoding").item(0);
//                                    if (node == null || !node.getTextContent().equals("base64")) {
//                                        System.out.println("supported only base64");
//                                        continue;
//                                    }
//                                    node = document.getElementsByTagName("content").item(0);
//                                    if (node == null) {
//                                        System.out.println("Null avatar file");
//                                        message = null;
//                                    }
                                    else {
                                        byte[] decodedBytes = Base64.getDecoder().decode(getTextContentByName(document, "content"));
//                                        byte[] decodedBytes = Base64.getDecoder().decode(node.getTextContent());

                                        FileOutputStream imageOutFile = new FileOutputStream("avatar" + id + ".png");
                                        imageOutFile.write(decodedBytes);
                                        imageOutFile.flush();
                                        imageOutFile.close();
                                        client.addAvatar(fNick, "avatar" + id + ".png");
                                        id++;
                                    }
                                }
                                default -> {
                                    System.out.println("Unknown event message");
                                    message = null;
                                }
                            }

                        } catch (IOException ex) {
                            throw new RuntimeException(ex);
                        }
                        catch (Exception e) {
                            System.out.println("Incorrect event message");
                            System.out.println(e.getMessage());
                            continue;
                        }
                    }
                    else {
                        System.out.println("Unknown type of message");
                        message = null;
                    }

                    client.onMadelChange();
                    System.out.println("output str: " + message);
                } catch (IOException e) {
                    System.out.println(e.getMessage());
                    break;
                } catch (SAXException e) {
                    throw new RuntimeException(e);
                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        System.out.println("read finished");
    }

    private String getTextContentByName(Document document, String name) {
        Node node = document.getElementsByTagName(name).item(0);
        if (node != null) {
            return node.getTextContent();
        }
        return null;
    }
    private Node getNodeByName(Document document, String name) {
        return document.getElementsByTagName(name).item(0);
    }
}
