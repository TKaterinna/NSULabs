module org.example.client {
    requires javafx.controls;
    requires javafx.fxml;
    requires java.xml;


    opens ru.nsu.testova to javafx.fxml;
    exports ru.nsu.testova;
    exports ru.nsu.testova.messages;
    opens ru.nsu.testova.messages to javafx.fxml;
    exports ru.nsu.testova.client;
    opens ru.nsu.testova.client to javafx.fxml;
}