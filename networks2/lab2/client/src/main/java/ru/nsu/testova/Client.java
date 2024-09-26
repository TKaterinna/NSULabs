package ru.nsu.testova;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.*;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import java.io.*;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;

public class Client {
    private String ip;
    private int port;
    private String filePath;

    public Client(String filePath, String ip, int port) {
        this.filePath = filePath;
        this.ip = ip;
        this.port = port;
    }

    public void start() {
        String fileName = Paths.get(filePath).getFileName().toString();

        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder;
        try {
            builder = factory.newDocumentBuilder();
        } catch (ParserConfigurationException e) {
            throw new RuntimeException(e);
        }
        Document document = builder.newDocument();

        Element name = document.createElement("name");
        name.appendChild(document.createTextNode(fileName));
        document.appendChild(name);

        try (Socket socket = new Socket(ip, port);
             DataOutputStream out = new DataOutputStream(socket.getOutputStream())) {
            byte[] mesBytes = serializeXMLDocument(document);
            out.writeInt(mesBytes.length);
            out.write(mesBytes, 0, mesBytes.length);

            InputStream file = Files.newInputStream(Paths.get(filePath));
            long fileSize = Files.size(Paths.get(filePath));
            out.writeLong(fileSize);
            int res = 0;
            while (true) {
                byte[] byteData = new byte[1024 * 1024];
                res = file.read(byteData, 0, 1024 * 1024);
                if (res == -1) {
                    break;
                }
                out.write(byteData, 0, res);
            }
            read(socket);
        } catch (IOException e) {
            throw new RuntimeException(e);
        } catch (TransformerException e) {
            throw new RuntimeException(e);
        }
    }

    public void read(Socket socket) {
        try (DataInputStream in = new DataInputStream(socket.getInputStream())) {
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            DocumentBuilder builder = null;
            Document document = null;
            try {
                builder = factory.newDocumentBuilder();
            } catch (ParserConfigurationException e) {
                throw new RuntimeException(e);
            }
            String message = null, type;
            byte[] messageB = new byte[8192];
            int res, lenMes, temp = 0;
            try {
                lenMes = in.readInt();
                if (lenMes > messageB.length) {
                    System.out.println("incorrect len of message (too big)");
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
                    System.out.println("incorrect len of message");
                }

                document = builder.parse(new ByteArrayInputStream(messageB, 0 , lenMes));
                type = document.getDocumentElement().getTagName();
                if (Objects.equals(type, "error")) {
                    try {
                        if (getNodeByName(document, "error") != null) {
                            message = getTextContentByName(document, "error");
                        }
                        else {
                            message = "error";
                        }
                    }
                    catch (Exception e) {
                        System.out.println("Incorrect error message");
                        System.out.println(e.getMessage());
                    }
                }
                else if (Objects.equals(type, "success")) {
                    try {
                        if (getNodeByName(document, "success") != null) {
                            message = "success " + getTextContentByName(document, "success");
                        }
                        else {
                            message = "success";
                        }
                    }
                    catch (Exception e) {
                        System.out.println("Incorrect success message");
                        System.out.println(e.getMessage());
                    }
                }
                else {
                    message = "Unknown type of message";
                }
                System.out.println(message);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
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

    private static byte[] serializeXMLDocument(Document doc) throws TransformerException {
        TransformerFactory transformerFactory = TransformerFactory.newInstance();
        Transformer transformer = transformerFactory.newTransformer();
        transformer.setOutputProperty(OutputKeys.ENCODING, "UTF-8");
        transformer.setOutputProperty(OutputKeys.INDENT, "yes");

        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        StreamResult result = new StreamResult(outputStream);
        DOMSource source = new DOMSource(doc);

        transformer.transform(source, result);

        return outputStream.toByteArray();
    }
}
