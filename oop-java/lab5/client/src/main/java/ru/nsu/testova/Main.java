package ru.nsu.testova;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.stage.Stage;
import ru.nsu.testova.client.Client;

public class Main extends Application {
    @Override
    public void start(Stage stage) {
        Client client = new Client();
//        Client client = new Client(8000, "45.142.36.163");//"localhost");//"45.142.36.163");
        Platform.runLater(() -> {
            MainWindow mainWindow = new MainWindow(client, stage);

        });
    }
    public static void main(String[] args) {
        launch();
    }
}