package ru.nsu.testova;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.xml.sax.SAXException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import java.io.*;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;

public class Connection implements Runnable {
    private final Socket clientSocket;
    private final Members currMember;
//    private PrintWriter out = null;
    private Server server = null;
    private DocumentBuilderFactory factory = null;
    private DocumentBuilder builder = null;
    public Connection(Socket clientSocket, Server server, Members currMember) {
        this.clientSocket = clientSocket;
        this.server = server;
        this.currMember = currMember;
    }
    @Override
    public void run() {
        DataInputStream in = null;
        try {
            in = new DataInputStream(clientSocket.getInputStream());
//            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        factory = DocumentBuilderFactory.newInstance();
        try {
            builder = factory.newDocumentBuilder();
        } catch (ParserConfigurationException e) {
            throw new RuntimeException(e);
        }
        byte[] messageSize = new byte[4];
//        byte[] messageB = new byte[78000000];
        int res, lenMes, temp = 0;
        while (true) {
            try {
//                String string = in.readLine();
//                res = in.read(messageSize, 0, 4);
                lenMes = in.readInt();
                if (lenMes == -1) {
                    continue; //?????????????????????????
                }
//                lenMes = ByteBuffer.wrap(messageSize).getInt();
                if (lenMes > 78000000) {
                    System.out.println(lenMes);
                    currMember.send("<error>to big len of message</error>");
                    continue;
                }
                byte[] messageB;
                try {
                    messageB = new byte[lenMes];
                }
                catch (Exception e) {
                    currMember.send("<error>to big len of message</error>");
                    continue;
                }
                res = 0;
                while (res != lenMes) {
                    temp = in.read(messageB, res, lenMes - res);
                    if (temp == -1) {
                        break; //?????????????????????????
                    }
                    res += temp;
                }
                if (temp == -1) {
                    currMember.send("<error>incorrect len of message</error>");
                    continue;
                }

                System.out.println(new String(messageB, 0, res, StandardCharsets.UTF_8));
                System.out.println(lenMes);

//                if (string != null && !clientSocket.isClosed()) {
                if (res > 0 && !clientSocket.isClosed()) {
                    try {
                        Document document = builder.parse(new ByteArrayInputStream(messageB, 0, lenMes));
                        makeCommand(document);
                    }
                    catch (Exception e) {
                        currMember.send("<error>" + e.getMessage() + "</error>");
                    }
                }
            } catch (IOException e) {
                System.out.println("IO\n" + e.getMessage());
                try {
                    in.close();
                } catch (IOException ex) {
                    throw new RuntimeException(ex);
                }
                break;
            } catch (Exception e) {
                System.out.println("E\n" + e.getMessage());
//                break;
            }
        }
    }

    private void makeCommand(Document message) {
        try {
            String commandName = message.getDocumentElement().getAttribute("name");
            System.out.println(commandName);
            switch (commandName) {
                case "login" -> login(message);
                case "message" -> message(message);
                case "logout" -> logout(message);
                case "list" -> list(message);
                case "upload" -> upload(message);
                case "download" -> download(message);
                case "avatar" -> setAvatar(message);
            }
        }
        catch (Exception e) {
            currMember.send("<error>" + e.getMessage() + "</error>");
        }
    }
    private void login(Document message) {
        try {
            String nick = "*no name*", password = null;
            if (getNodeByName(message, "name") != null) {
                nick = getTextContentByName(message, "name");
            }
            if (getNodeByName(message, "password") != null) {
                password = getTextContentByName(message, "password");
            }
//            Node node = message.getElementsByTagName("name").item(0);
//            if (node != null) {
//                nick = node.getTextContent();
//            }
//            node = message.getElementsByTagName("password").item(0);
//            if (node != null) {
//                password = node.getTextContent();
//            }
            String retConnection = server.findMemberCheckPassword(nick, password, currMember);
            if (retConnection == null) {
                System.out.println("client connected");
                currMember.send("<success></success>");
                for (String mes : server.getLastMessages()) {
                    currMember.send(mes);
                }
                Map<String, String> map = server.getAvatars();
                if (map != null) {
                    for (String mes : map.values()) {
                        currMember.send(mes);
                    }
                }
                server.sendExcept("<event name=\"userlogin\"><name>" + nick + "</name></event>", currMember);
            } else {
                System.out.println("retConnection: " + retConnection);
                currMember.send("<error><message>" + retConnection + "</message></error>");
            }
        }
        catch (Exception e) {
            currMember.send("<error>" + e.getMessage() + "</error>");
        }
    }
    private void list(Document message) {
        try {
            List<String> list = server.getListMembers();
            StringBuilder stringBuilder = new StringBuilder("<success><users>");
            for (String nick : list) {
                stringBuilder.append("<user><name>").append(nick).append("</name></user>");
            }
            stringBuilder.append("</users></success>");

            currMember.send(stringBuilder.toString());
        }
        catch (Exception e) {
            currMember.send("<error>" + e.getMessage() + "</error>");
        }
    }
    private void logout(Document message) {
        try {
            currMember.send("<success></success>");
            currMember.closeClient();

            server.removeMember(currMember);

            System.out.println(currMember.getNick() + " disconnected");
            server.sendAll("<event name=\"userlogout\"><name>" + currMember.getNick() + "</name></event>");
        }
        catch (Exception e) {
            currMember.send("<error>" + e.getMessage() + "</error>");
        }
    }
    private void message(Document message) {
        try {
            String text = null;
            if (getNodeByName(message, "message") != null) {
                text = getTextContentByName(message, "message");
            }
//            Node node = message.getElementsByTagName("message").item(0);
//            if (node != null) {
//                text = node.getTextContent();
//            }
            server.sendAll("<event name=\"message\"><from>" + currMember.getNick() + "</from><message>" + text + "</message></event>");
            currMember.send("<success></success>");
            server.addMessage("<event name=\"message\"><from>" + currMember.getNick() + "</from><message>" + text + "</message></event>");
        }
        catch (Exception e) {
            currMember.send("<error>" + e.getMessage() + "</error>");
        }
    }
    private void upload(Document message) {
        try {
            String fileName = "*no name*";
            byte[] decodedBytes = new byte[0];
            int fileSize = decodedBytes.length;
            String mimeType = "*no type*";
            if (getNodeByName(message, "name") != null) {
                fileName = getTextContentByName(message, "name");
            }
            if (getNodeByName(message, "mimeType") != null) {
                mimeType = getTextContentByName(message, "mimeType");
            }
            if (getNodeByName(message, "content") != null) {
                decodedBytes = Base64.getDecoder().decode(getTextContentByName(message, "content"));
            }
            if (getNodeByName(message, "encoding") == null || !getTextContentByName(message, "encoding").equals("base64")) {
                currMember.send("<error><message>supported only base64</message></error>");
                return;
            }
//            Node node = message.getElementsByTagName("name").item(0);
//            if (node != null) {
//                fileName = node.getTextContent();
//            }
//            node = message.getElementsByTagName("mimeType").item(0);
//            if (node != null) {
//                mimeType = node.getTextContent();
//            }
//            node = message.getElementsByTagName("content").item(0);
//            if (node != null) {
//                decodedBytes = Base64.getDecoder().decode(node.getTextContent());
//            }
//            node = message.getElementsByTagName("encoding").item(0);
//            if (node == null || !node.getTextContent().equals("base64")) {
//                currMember.send("<error><message>supported only base64</message></error>");
//                return;
//            }

            int id = server.saveFile("<name>" + fileName + "</name><mimeType>" + mimeType + "</mimeType><encoding>base64</encoding>");

            File directory = new File("files");

            if (!directory.exists()) {
                boolean isDirectoryCreated = directory.mkdir();

                if (isDirectoryCreated) {
                    System.out.println("Директория создана успешно");
                } else {
                    System.out.println("Не удалось создать директорию");
                }
            } else {
                System.out.println("Директория уже существует");
            }

            FileOutputStream fos = new FileOutputStream("files/" + id);
            fos.write(decodedBytes);
            fos.flush();
            fos.close();

            server.sendExcept("<event name=\"file\"><id>" + id + "</id><from>" + currMember.getNick() + "</from><name>" + fileName + "</name><size>" + fileSize + "</size><mimeType>" + mimeType + "</mimeType></event>", currMember);
            currMember.send("<success><id>" + id + "</id></success>");
            server.addMessage("<event name=\"file\"><id>" + id + "</id><from>" + currMember.getNick() + "</from><name>" + fileName + "</name><size>" + fileSize + "</size><mimeType>" + mimeType + "</mimeType></event>");
        }
        catch (Exception e) {
            currMember.send("<error>" + e.getMessage() + "</error>");
        }
    }
    private void download(Document message) {
        try {
            int id = -1;
            if (getNodeByName(message, "id") != null) {
                id = Integer.parseInt(getTextContentByName(message, "id"));
            }
//            Node node = message.getElementsByTagName("id").item(0);
//            if (node != null) {
//                id = Integer.parseInt(node.getTextContent());
//            }
            else {
                currMember.send("<error>no file id</error>");
            }
            String doc = server.getFile(id);

            byte[] byteData = new byte[0];
            try {
                byteData = Files.readAllBytes(Paths.get("files/" + id));
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
            String base64Text = Base64.getEncoder().encodeToString(byteData);
            currMember.send("<success><id>" + id + "</id>" + doc + "<content>" + base64Text + "</content></success>");
        }
        catch (Exception e) {
            currMember.send("<error>broken file</error>");
        }
    }
    private void setAvatar(Document message) {
        try {
            if (getNodeByName(message, "encoding") == null) {
                currMember.send("<error><message>no type of encoding</message></error>");
                return;
            }
            if (!getTextContentByName(message, "encoding").equals("base64")) {
                currMember.send("<error><message>supported only base64</message></error>");
                return;
            }
            String base64Text, mimeType = null;
            if (getNodeByName(message, "content") != null) {
                base64Text = getTextContentByName(message, "content");
            }
            else {
                currMember.send("<error><message>null encoding file</message></error>");
                return;
            }
            if (getNodeByName(message, "mimeType") != null) {
                mimeType = getTextContentByName(message, "mimeType");
            }

//            Node node = message.getElementsByTagName("encoding").item(0);
//            if (node == null) {
//                currMember.send("<error><message>no type of encoding</message></error>");
//                return;
//            }
//            if (!node.getTextContent().equals("base64")) {
//                currMember.send("<error><message>supported only base64</message></error>");
//                return;
//            }
//            String base64Text, mimeType = null;
//            node = message.getElementsByTagName("content").item(0);
//            if (node == null) {
//                currMember.send("<error><message>null encoding file</message></error>");
//                return;
//            }
//            base64Text = node.getTextContent();
//            node = message.getElementsByTagName("mimeType").item(0);
//            if (node != null) {
//                mimeType = node.getTextContent();
//            }
            server.addAvatar(currMember.getNick(), "<event name=\"avatar\"><from>" + currMember.getNick() + "</from><mimeType>" + mimeType + "</mimeType><encoding>base64</encoding><content>" + base64Text + "</content></event>");
            server.sendAll("<event name=\"avatar\"><from>" + currMember.getNick() + "</from><mimeType>" + mimeType + "</mimeType><encoding>base64</encoding><content>" + base64Text + "</content></event>");
            currMember.send("<success></success>");
        }
        catch (Exception e) {
            currMember.send("<error>" + e.getMessage() + "</error>");
        }
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
