package ru.nsu.testova.client;

import javafx.scene.image.Image;
import ru.nsu.testova.ModelListener;
import ru.nsu.testova.messages.IMessage;
import ru.nsu.testova.messages.UserFile;

import java.io.*;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Client implements AutoCloseable {
    private Socket socket = null;
    private ExecutorService service = null;
    private DataOutputStream out = null;
    private ModelListener listener;
    private volatile boolean successFlag = false;
    private volatile boolean errorFlag = false;
    private List<IMessage> messageList = null;
    private List<IMessage> errorsList = null;
    private Queue<File> dirsForDownload = null;
    private volatile Image avatar = null;
    private Map<String, Image> avatars = null;
    private int port = 0;
    private String ip;
    private final Object lockM = new Object();
    private final Object lockE = new Object();
    private final Object lockD = new Object();
    private final Object lockA = new Object();

    public Client() {
        messageList = new ArrayList<>();
        errorsList = new ArrayList<>();
        dirsForDownload = new LinkedList<>();
        avatar = new Image(Objects.requireNonNull(getClass().getResourceAsStream("/ru/nsu/testova/avatar.png")));
        avatars = new HashMap<>();
    }
    public void setPort(int port) {
        this.port = port;
    }
    public void setIp(String ip) {
        this.ip = ip;
    }

    public Client(int port, String ip) {
        messageList = new ArrayList<>();
        errorsList = new ArrayList<>();
        dirsForDownload = new LinkedList<>();
        avatar = new Image(Objects.requireNonNull(getClass().getResourceAsStream("/ru/nsu/testova/avatar.png")));
        avatars = new HashMap<>();
        this.ip = ip;
        this.port = port;
    }
    public void connect() {
        try {
            socket = new Socket(ip, port);
            out = new DataOutputStream(socket.getOutputStream());
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        ReadThread readThread = new ReadThread(socket, this);
        service = Executors.newCachedThreadPool();
        service.execute(readThread);
    }
    public void disconnect() {
        try {
            out.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        service.shutdownNow();
        System.out.println("disconnect " + service.isShutdown());
        try {
            socket.close();
            socket = null;
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        port = 0;
        service = null;
    }
    public synchronized void setListener(ModelListener listener) {
        this.listener = listener;
    }
    public void login(String nick, String password) {
        String mes = "<command name=\"login\"><name>" + nick + "</name><password>" + password + "</password></command>";
        try {
//            out.write(ByteBuffer.allocate(4).putInt(mes.getBytes().length).array());
            out.writeInt(mes.getBytes().length);
            out.write(mes.getBytes(), 0, mes.getBytes().length);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
//        out.println("<command name=\"login\"><name>" + nick + "</name><password>" + password + "</password></command>");
    }
    public void message(String message) {
        String mes = "<command name=\"message\"><message>" + message + "</message></command>";
        try {
//            out.write(ByteBuffer.allocate(4).putInt(mes.getBytes().length).array());
            out.writeInt(mes.getBytes().length);
            out.write(mes.getBytes(), 0, mes.getBytes().length);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    public void list() {
        String mes = "<command name=\"list\"></command>";
        try {
//            out.write(ByteBuffer.allocate(4).putInt(mes.getBytes().length).array());
            out.writeInt(mes.getBytes().length);
            out.write(mes.getBytes(), 0, mes.getBytes().length);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
//        out.println("<command name=\"list\"></command>");
    }
    public void logout() {
        String mes = "<command name=\"logout\"></command>";
        try {
//            out.write(ByteBuffer.allocate(4).putInt(mes.getBytes().length).array());
            out.writeInt(mes.getBytes().length);
            out.write(mes.getBytes(), 0, mes.getBytes().length);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
//        out.println("<command name=\"logout\"></command>");
    }
    public void upload(File file) {
        String mimoType;
        try {
            mimoType = Files.probeContentType(Paths.get(file.getPath()));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        byte[] byteData = new byte[0];
        try {
            byteData = Files.readAllBytes(Paths.get(file.getPath()));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        String base64Text = Base64.getEncoder().encodeToString(byteData);

        String mes = "<command name=\"upload\"><name>" + file.getName() + "</name><mimeType>" + mimoType + "</mimeType><encoding>base64</encoding><content>" + base64Text + "</content></command>";
//        System.out.println(mes);
//        System.out.println(mes.getBytes().length);
        try {
//            out.write(ByteBuffer.allocate(4).putInt(mes.getBytes().length).array());
            out.writeInt(mes.getBytes().length);
            out.write(mes.getBytes(), 0, mes.getBytes().length);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    public void download(UserFile file) {
        String mes = "<command name=\"download\"><id>" + file.getId() + "</id></command>";
        try {
//            out.write(ByteBuffer.allocate(4).putInt(mes.getBytes().length).array());
            out.writeInt(mes.getBytes().length);
            out.write(mes.getBytes(), 0, mes.getBytes().length);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    public void setAvatar(File file) {
        String mimoType;
        try {
            mimoType = Files.probeContentType(Paths.get(file.getPath()));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        byte[] byteData = new byte[0];
        try {
            byteData = Files.readAllBytes(Paths.get(file.getPath()));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        String base64Text = Base64.getEncoder().encodeToString(byteData);

        String mes = "<command name=\"avatar\"><mimeType>" + mimoType + "</mimeType><encoding>base64</encoding><content>" + base64Text + "</content></command>";
//        System.out.println(mes);
//        System.out.println(mes.length());
        try {
//            System.out.println("len message " + mes.length());
//            System.out.println(Arrays.toString(ByteBuffer.allocate(4).putInt(mes.length()).array()));
//            out.write(ByteBuffer.allocate(4).putInt(mes.getBytes().length).array());
            out.writeInt(mes.getBytes().length);
            out.write(mes.getBytes(), 0, mes.getBytes().length);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    @Override
    public void close() throws InterruptedException {
        if (port != 0) {
            disconnect();
        }
    }

    public void setErrorFlag(boolean errorFlag) {
        this.errorFlag = errorFlag;
    }
    public void setSuccessFlag(boolean successFlag) {
        this.successFlag = successFlag;
    }
    public boolean isErrorFlag() {
        return errorFlag;
    }
    public boolean isSuccessFlag() {
        return successFlag;
    }
    public List<IMessage> getMessageList() {
        synchronized (lockM) {
            return new ArrayList<>(messageList);
        }
    }
    public void addMessageList(IMessage x) {
        synchronized (lockM) {
            messageList.add(x);
        }
    }
    public List<IMessage> getErrorsList() {
        synchronized (lockE) {
            return new ArrayList<>(errorsList);
        }
    }
    public void addErrorsList(IMessage x) {
        synchronized (lockM) {
            errorsList.add(x);
        }
    }
    public void onMadelChange() {
        listener.onModelChanged();
    }
    public void addDir(File dir) {
        synchronized (lockD) {
            dirsForDownload.add(dir);
        }
    }
    public File getDir() {
        synchronized (lockD) {
            return dirsForDownload.poll();
        }
    }
    public void addAvatar(String nick, String fileName) throws FileNotFoundException {
        Image image = new Image(new FileInputStream(fileName));
        synchronized (lockA) {
            if (avatars.containsKey(nick)) {
                avatars.replace(nick, image);
            } else {
                avatars.put(nick, image);
            }
        }
    }
    public Image getAvatar(String nick) {
        synchronized (lockA) {
            return avatars.get(nick);
        }
    }
    public Image getDefaultAvatar() {
        return avatar;
    }
}
