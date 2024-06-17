module ru.nsu.testova.lab3 {
    requires javafx.controls;
    requires javafx.fxml;
    requires java.desktop;


    opens ru.nsu.testova to javafx.fxml;
    //exports ru.nsu.testova;
    exports ru.nsu.testova.model;
    opens ru.nsu.testova.model to javafx.fxml;
    exports ru.nsu.testova.viewjavafx;
    opens ru.nsu.testova.viewjavafx to javafx.fxml;
}