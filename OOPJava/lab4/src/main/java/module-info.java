module ru.nsu.testova.lab4 {
    requires javafx.controls;
    requires javafx.fxml;


    opens ru.nsu.testova.lab4 to javafx.fxml;
    exports ru.nsu.testova.lab4;
    exports ru.nsu.testova.lab4.model.storage;
    opens ru.nsu.testova.lab4.model.storage to javafx.fxml;
}