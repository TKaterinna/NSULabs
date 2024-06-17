package ru.nsu.testova.lab4;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.stage.Stage;


public class MainJavaFX extends Application {
    @Override
    public void start(Stage stage) {
        Platform.runLater(() -> {
            MainWindow mainWindow = new MainWindow(stage);
        });
    }

    public static void main(String[] args) {
        launch();
    }
}