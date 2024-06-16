package ru.nsu.testova;

import javafx.application.Platform;
import javafx.beans.property.DoubleProperty;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import ru.nsu.testova.client.Client;
import ru.nsu.testova.messages.IMessage;
import ru.nsu.testova.messages.Message;
import ru.nsu.testova.messages.TypeMessage;
import ru.nsu.testova.messages.UserFile;

import java.io.File;
import java.util.List;

public class MainWindow implements ModelListener {
    private final Client client;
    private final Stage stage;
    private VBox chatRoot;
    private VBox loginRoot;
    private Scene chatScene = null;
    private Scene loginScene = null;
    private TextField tfNick, tfPassword, tfMessage, tfPort, tfIp;
    private boolean loginWait = false;
    private boolean logoutWait = false;
    private ListView<HBox> LVChat;
    public MainWindow(Client client, Stage stage) {
        this.client = client;
        this.stage = stage;
        client.setListener(this);
        stage.setOnCloseRequest(new EventHandler<WindowEvent>() {
            @Override public void handle(WindowEvent t) {
                try {
                    client.close();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        });
        stage.setTitle("Messenger\n");
        stage.setResizable(false);

        makeLoginScene();
        makeChatScene();
        setLoginScene();
    }

    public void makeLoginScene() {
        tfPort = new TextField("8000");
        tfPort.setStyle("-fx-font-size: 30");
        tfIp = new TextField("45.142.36.163");
        tfIp.setStyle("-fx-font-size: 30");

        tfNick = new TextField("nick");
        tfNick.setStyle("-fx-font-size: 30");
        tfPassword = new TextField("password");
        tfPassword.setStyle("-fx-font-size: 30");
        Button bLogin = new Button("Login");
        bLogin.setStyle("-fx-font-size: 30");

        loginRoot = new VBox(tfPort, tfIp, tfNick, tfPassword, bLogin);
        loginRoot.setAlignment(Pos.CENTER);
        loginScene = new Scene(loginRoot, 850, 650);

        bLogin.setPrefWidth(200);
        bLogin.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
//                if (!tfNick.getText().isEmpty() && !tfPassword.getText().isEmpty()) {
                client.setPort(Integer.parseInt(tfPort.getText()));
                client.setIp(tfIp.getText());
                client.connect();
                    loginWait = true;
                    client.login(tfNick.getText(), tfPassword.getText());
//                }
            }
        });
    }
    public void makeChatScene() {
        tfMessage = new TextField("message");
        tfNick.setStyle("-fx-font-size: 30");
        Button bSend = new Button("Send");
        bSend.setStyle("-fx-font-size: 30");
        Button bList = new Button("List");
        bList.setStyle("-fx-font-size: 30");
        Button bLogout = new Button("Logout");
        bLogout.setStyle("-fx-font-size: 30");
        LVChat = new ListView<>();
        LVChat.setMinSize(400, 300);

        FileChooser fileChooser = new FileChooser();
        Button bFile = new Button("Select File");
        bFile.setPrefWidth(300);
        bFile.setStyle("-fx-font-size: 30");
        bFile.setOnAction(e -> {
            if (client.isSuccessFlag() || client.isErrorFlag()) {
                File selectedFile = fileChooser.showOpenDialog(stage);
                if (selectedFile != null) {
                    client.setSuccessFlag(false);
                    client.setErrorFlag(false);
                    client.upload(selectedFile);
                }
            }
        });

        Button bAvatar = new Button("Change avatar");
        bAvatar.setStyle("-fx-font-size: 30");
        bAvatar.setPrefWidth(300);
        bAvatar.setOnAction(e -> {
            if (client.isSuccessFlag() || client.isErrorFlag()) {
                File selectedFile = fileChooser.showOpenDialog(stage);
                if (selectedFile != null) {
                    client.setSuccessFlag(false);
                    client.setErrorFlag(false);
                    client.setAvatar(selectedFile);
                }
            }
        });

        bSend.setPrefWidth(300);
        bSend.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                if (client.isSuccessFlag() || client.isErrorFlag()) {
                    client.setSuccessFlag(false);
                    client.setErrorFlag(false);
//                if (!tfMessage.getText().isEmpty()) {
                    client.message(tfMessage.getText());
                    tfMessage.clear();
//                }
                }
            }
        });
        bList.setPrefWidth(300);
        bList.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                if (client.isSuccessFlag() || client.isErrorFlag()) {
                    client.setSuccessFlag(false);
                    client.setErrorFlag(false);
                    client.list();
                }
            }
        });
        bLogout.setPrefWidth(300);
        bLogout.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                if (client.isSuccessFlag() || client.isErrorFlag()) {
                    client.setSuccessFlag(false);
                    client.setErrorFlag(false);
                    logoutWait = true;
                    client.logout();
                }
            }
        });

        Button bE1 = new Button("⍝◜ᐢ•_•ᐢ◝⍝");
        bE1.setStyle("-fx-font-size: 30");
        bE1.setOnAction(e -> {
            tfMessage.appendText("⍝◜ᐢ•_•ᐢ◝⍝");
        });
        Button bE2 = new Button("\uD83D\uDE04");
        bE2.setStyle("-fx-font-size: 30");
        bE2.setOnAction(e -> {
            tfMessage.appendText("\uD83D\uDE04");
        });
        Button bE3 = new Button("\uD83D\uDE08");
        bE3.setStyle("-fx-font-size: 30");
        bE3.setOnAction(e -> {
            tfMessage.appendText("\uD83D\uDE08");
        });
        HBox box = new HBox(bE1, bE2, bE3, bSend);

        chatRoot = new VBox(bLogout, LVChat, tfMessage, box, bList, bFile, bAvatar);
        chatRoot.setAlignment(Pos.CENTER);
        chatScene = new Scene(chatRoot, 850, 650);
    }
    public void setLoginScene() {
        Platform.runLater(() -> {
//            client.connect();
            stage.setScene(loginScene);
            stage.show();
        });
    }
    public void setChatScene() {
        Platform.runLater(() -> {
            if (chatScene != null) {
                stage.setScene(chatScene);
                stage.show();
            }
        });
    }

    @Override
    public void onModelChanged() {
        Platform.runLater(() -> {
            if (loginWait && client.isErrorFlag()) {
//                loginWait = false;
                client.disconnect();
            }
            if (loginWait && client.isSuccessFlag()) {
                loginWait = false;
//                client.setSuccessFlag(false);
                setChatScene();
            }
            if (logoutWait && client.isSuccessFlag()) {
                logoutWait = false;
                client.setSuccessFlag(false);
                client.disconnect();
                client.getMessageList().clear();
                setLoginScene();
                return;
            }
            LVChat.getItems().clear();
            List<IMessage> listM = client.getMessageList();
            for (IMessage m : listM) {
                if (m.getType() == TypeMessage.FILE) {
                    Label label = new Label(m.getMassage() + "\n");
                    label.setOnMouseClicked(e -> {
                        DirectoryChooser directoryChooser = new DirectoryChooser();
                        File dir = directoryChooser.showDialog(stage);
                        if (dir != null) {
                            client.addDir(dir);
                            client.download((UserFile) m);
                        }
                    });
                    LVChat.getItems().add(new HBox(label));
                }
                else {
                    if (m.getType() == TypeMessage.MESSAGE) {
                        if (client.getAvatar(((Message) m).getNick()) == null) {
                            ImageView imageView = new ImageView(client.getDefaultAvatar());
                            imageView.setFitHeight(50);
                            imageView.setFitWidth(50);
//                            DoubleProperty doubleProperty = imageView.fitWidthProperty();
//                            doubleProperty.set(40);
                            LVChat.getItems().add(new HBox(imageView, new Label(m.getMassage() + "\n")));
                        } else {
                            ImageView imageView = new ImageView(client.getAvatar(((Message) m).getNick()));
                            imageView.setFitHeight(50);
                            imageView.setFitWidth(50);
//                            DoubleProperty doubleProperty = imageView.fitWidthProperty();
//                            doubleProperty.set(40);
                            LVChat.getItems().add(new HBox(imageView, new Label(m.getMassage() + "\n")));
                        }
                    }
                    else {
                        LVChat.getItems().add(new HBox(new Label(m.getMassage() + "\n")));
                    }
                }
            }

            if (client.isErrorFlag()) {
                List<IMessage> errorList = client.getErrorsList();
                for (IMessage iMessage : errorList) {
                    Alert alert = new Alert(Alert.AlertType.ERROR);
                    alert.setTitle("Error");
                    alert.setContentText(iMessage.getMassage());
                    alert.showAndWait();
                }
                client.getErrorsList().clear();
                if (loginWait && client.isErrorFlag()) {
                    loginWait = false;
                    client.setErrorFlag(false);
                }
            }
        });
    }
}
