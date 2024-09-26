package ru.nsu.testova;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import java.io.*;
import java.net.Socket;
import java.nio.file.Paths;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

public class Connection implements Runnable {
    private final Socket clientSocket;
    private final AtomicInteger bytesCount = new AtomicInteger(0);
    private final AtomicInteger allBytesCount = new AtomicInteger(0);
    private AtomicLong lastTime = new AtomicLong(0);
    private AtomicLong startTime = new AtomicLong(0);

    public Connection(Socket clientSocket) {
        this.clientSocket = clientSocket;
//        try { // убрать из поля в локалку в try with resources в run
//            out = new DataOutputStream(clientSocket.getOutputStream());
//        } catch (IOException e) {
//            throw new RuntimeException(e);
//        }
    }

    @Override
    public void run() {
        int delay = 3000;
        long tid = Thread.currentThread().getId();
        Timer timer = new Timer();
        TimerTask task = new TimerTask() {
            @Override
            public void run() {
                long count = bytesCount.get();
                double result = (double) count / delay * 1000 / 1024;
                System.out.println("User " + tid + ": " + result + " kb/sec (moment)");

                result = (double) (allBytesCount.get()) / (System.currentTimeMillis() - startTime.get()) * 1000 / 1024;
                System.out.println("User " + tid + ": " + result + " kb/sec (average)");

                bytesCount.set(0);
                lastTime.set(System.currentTimeMillis());
            }
        };

        DocumentBuilderFactory factoryRead = DocumentBuilderFactory.newInstance();
        DocumentBuilder builderRead;
        try {
            builderRead = factoryRead.newDocumentBuilder();
        } catch (ParserConfigurationException e) {
            throw new RuntimeException(e);
        }

        int res = 0, lenMes = 0, temp = 0;
        long lenFile = 0;

        try (DataOutputStream out = new DataOutputStream(clientSocket.getOutputStream());
             DataInputStream in = new DataInputStream(clientSocket.getInputStream())) {
            byte[] messageB;
            lenMes = in.readInt();
            if (lenMes == -1) {
                close(timer);
                return;
            }
            if (lenMes > 8192) {
                send(out, "error", "to big len of message");
                close(timer);
                return;
            }
            try {
                messageB = new byte[lenMes];
            } catch (Exception e) {
                send(out, "error", "to big len of message");
                close(timer);
                return;
            }
            while (res != lenMes) {
                temp = in.read(messageB, res, lenMes - res);
                if (temp == -1) {
                    break;
                }
                res += temp;
            }
            if (temp == -1) {
                send(out, "error", "incorrect len of message");
                close(timer);
                return;
            }

            Document documentRead = null;
            if (res > 0 && !clientSocket.isClosed()) {
                try {
                    documentRead = builderRead.parse(new ByteArrayInputStream(messageB, 0, lenMes));
                } catch (Exception e) {
                    send(out, "error", e.getMessage());
                    close(timer);
                    return;
                }
            } else {
                close(timer);
                return;
            }

            String fileName = makeFile(documentRead);
            if (fileName == null) {
                send(out, "error", "incorrect message");
                close(timer);
                return;
            }
            try (FileOutputStream fos = new FileOutputStream(fileName)) {
                lenFile = in.readLong();
                if (lenFile == -1) {
                    close(timer);
                    return;
                }
                System.out.println(lenFile);
                if (lenFile > 1099511627776L) {
                    send(out, "error", "to big len of message");
                    close(timer);
                    return;
                }

                long time = System.currentTimeMillis();
                startTime.set(time);
                lastTime.set(time);
                timer.scheduleAtFixedRate(task, delay, delay);

                while (allBytesCount.get() < lenFile) {
                    try {
                        messageB = new byte[1024 * 1024];
                    } catch (Exception e) {
                        send(out, "error", "cannot make a buffer");
                        continue;
                    }
                    temp = in.read(messageB, 0, 1024 * 1024);
                    if (temp == -1) {
                        break;
                    }
                    allBytesCount.addAndGet(temp);
                    bytesCount.addAndGet(temp);
                    fos.write(messageB, 0, temp);
                }
                if (allBytesCount.get() != lenFile) {
                    send(out, "error", "incorrect len of message");
                    close(timer);
                    return;
                }

                fos.flush();
                send(out, "success", fileName);
            } catch (Exception e) {
                close(timer);
            }
        } catch (Exception e) {
            System.out.println("E\n" + e.getMessage());
            close(timer);
        }

        timer.cancel();
        long currentTime = System.currentTimeMillis();
        double result = (double) bytesCount.get() / (currentTime - lastTime.get()) * 1000 / 1024;
        System.out.println("User " + tid + ": " + result + " kb/sec (moment)");
        result = (double) (allBytesCount.get()) / (currentTime - startTime.get()) * 1000 / 1024;
        System.out.println("User " + tid + ": " + result + " kb/sec (average)");
    }

    private String makeFile(Document message) {
        String fileName;
        if (getNodeByName(message, "name") != null) {
            fileName = getTextContentByName(message, "name");
            fileName = Paths.get(fileName).getFileName().toString();
        }
        else {
            return null;
        }

        File directory = new File("uploads");

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

        File file = new File("uploads/" + fileName);

        while (file.exists()) {
            if (fileName.lastIndexOf('.') == -1) {
                fileName += "(1)";
            }
            else {
                fileName = fileName.substring(0, fileName.lastIndexOf('.')) + "(1)" + fileName.substring(fileName.lastIndexOf('.'));
            }
            file = new File("uploads/" + fileName);
        }

        return ("uploads/" + fileName);
    }

    public void send(DataOutputStream out, String state, String message) {
        if (out != null) {
            try {
                DocumentBuilderFactory factorySend = DocumentBuilderFactory.newInstance();
                DocumentBuilder builderSend;
                try {
                    builderSend = factorySend.newDocumentBuilder();
                } catch (ParserConfigurationException e) {
                    throw new RuntimeException(e);
                }
                Document documentSend = builderSend.newDocument();
//                Element name = documentSend.createElement(state);
//                name.setNodeValue(message);
//                documentSend.appendChild(name);
                Element name = documentSend.createElement(state);
                name.appendChild(documentSend.createTextNode(message));
                documentSend.appendChild(name);

                byte[] messageBytes = serializeXMLDocument(documentSend);

                out.writeInt(messageBytes.length);
                out.write(messageBytes, 0, messageBytes.length);
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
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

    private void close(Timer timer) {
        try {
            timer.cancel();
            clientSocket.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
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
