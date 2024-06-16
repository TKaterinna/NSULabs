package ru.nsu.testova.viewjavafx;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.stage.Stage;
import ru.nsu.testova.model.Model;

import java.io.IOException;

public class MainJavaFX extends Application {
    @Override
    public void start(Stage stage) throws IOException {
        Model model = new Model();
        Platform.runLater(() -> {
            MainWindowJavaFX mainWindow = new MainWindowJavaFX(model, stage);
//            ControllerJavaFX controller = new ControllerJavaFX(mainWindow, model);
//            controller.startGame();
        });
//        FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
//        Scene scene = new Scene(fxmlLoader.load(), 400, 600);
//        stage.setTitle("Hello!");
//        stage.setScene(scene);
//        stage.show();
    }

    public static void main(String[] args) {
        launch();
    }
}