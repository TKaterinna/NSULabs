package ru.nsu.testova.lab4;

import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Orientation;
import javafx.scene.Scene;
import javafx.scene.control.Slider;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import ru.nsu.testova.lab4.controller.ControllerCarStorage;
import ru.nsu.testova.lab4.model.ListenerSpeed;
import ru.nsu.testova.lab4.model.ModelListener;
import ru.nsu.testova.lab4.model.dealer.DealerF;
import ru.nsu.testova.lab4.model.storage.CarStorage;
import ru.nsu.testova.lab4.model.storage.StorageF;
import ru.nsu.testova.lab4.model.supplier.AccessorySupplier;
import ru.nsu.testova.lab4.model.supplier.BodySupplier;
import ru.nsu.testova.lab4.model.supplier.EngineSupplier;
import ru.nsu.testova.lab4.model.supplier.WorkerF;
import threadpool.MThreadPool;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.*;

public class MainWindow implements ModelListener {
    private Text tCarStorage, tBodyStorage, tEngineStorage, tAccessoryStorage, tQueueCount;
    private int S = 10;
    private int D = 2;
    private int W = 10;
    private int timeSupplier = 1000;
    private int timeDealer = 5000;
    private int maxCarStorage = 5;
    private int maxAccessoryStorage = 10;
    private int maxEngineStorage = 7;
    private int maxBodyStorage = 3;
    private CarStorage carStorage;
    private StorageF bodyStorage, accessoryStorage, engineStorage;
    public MainWindow(Stage stage) {

        parseConfig();

        Clock clock = new Clock();
        carStorage = new CarStorage(this, maxCarStorage);
        bodyStorage = new StorageF(this, maxBodyStorage);
        accessoryStorage = new StorageF(this, maxAccessoryStorage);
        engineStorage = new StorageF(this, maxEngineStorage);

        List<WorkerF> workers = new ArrayList<>();
        for (int i = 0; i < W; i++) {
            workers.add(new WorkerF(i, engineStorage, carStorage, bodyStorage, accessoryStorage));
        }

        ControllerCarStorage controllerCarStorage = new ControllerCarStorage(carStorage, workers);
        carStorage.setListener(controllerCarStorage);

        BlockingQueue<Runnable> queueS = new ArrayBlockingQueue<>(30);


//        ExecutorService serviceSupplier = new MExecutorService(2 + S, queueS);
        ExecutorService serviceSupplier = new MThreadPool(2 + S, queueS);


        //ExecutorService serviceSupplier = Executors.newCachedThreadPool();
        BodySupplier bodySupplier = new BodySupplier(this, bodyStorage, timeSupplier);
        ListenerSpeed bListenerSpeed = new ListenerSpeed(bodySupplier);
        serviceSupplier.submit(bodySupplier);
//        serviceSupplier.execute(bodySupplier);

        EngineSupplier engineSupplier = new EngineSupplier(this, engineStorage, timeSupplier);
        ListenerSpeed eListenerSpeed = new ListenerSpeed(engineSupplier);
        serviceSupplier.submit(engineSupplier);
//        serviceSupplier.execute(engineSupplier);

        List<ListenerSpeed> aListenerSpeed = new ArrayList<>();
        for (int i = 0; i < S; i++) {
            AccessorySupplier accessorySupplier = new AccessorySupplier(this, accessoryStorage, i, timeSupplier);
            aListenerSpeed.add(new ListenerSpeed(accessorySupplier));
            serviceSupplier.submit(accessorySupplier);
//            serviceSupplier.execute(accessorySupplier);
        }

        BlockingQueue<Runnable> queueD = new ArrayBlockingQueue<>(30);


//        ExecutorService serviceDealer = new MExecutorService(D, queueD);
        ExecutorService serviceDealer = new MThreadPool(D, queueD);



        //ExecutorService serviceDealer = Executors.newCachedThreadPool();
        List<ListenerSpeed> dListenerSpeed = new ArrayList<>();
        for (int i = 0; i < D; i++) {
            DealerF dealerF = new DealerF(carStorage, i, timeDealer, clock);
            dListenerSpeed.add(new ListenerSpeed(dealerF));
            serviceDealer.submit(dealerF);
//            serviceDealer.execute(dealerF);
        }

        tBodyStorage = new Text( "Body storage: 0/" + maxBodyStorage + " Count: 0");
        tBodyStorage.setStyle("-fx-font-size: 30");
        tEngineStorage = new Text("Engine storage: 0/" + maxEngineStorage + " Count: 0");
        tEngineStorage.setStyle("-fx-font-size: 30");
        tAccessoryStorage = new Text("Accessory storage: 0/" + maxAccessoryStorage + " Count: 0");
        tAccessoryStorage.setStyle("-fx-font-size: 30");
        tCarStorage = new Text("Car storage: 0/" + maxCarStorage + " Count: 0");
        tCarStorage.setStyle("-fx-font-size: 30");
        tQueueCount = new Text( "Queue: 0");
        tQueueCount.setStyle("-fx-font-size: 30");



        Slider sSupplierB = new Slider(500, 10000, timeSupplier);
        sSupplierB.setOrientation(Orientation.HORIZONTAL);
        sSupplierB.setMajorTickUnit(500);
        sSupplierB.setMinorTickCount(0);
        sSupplierB.setSnapToTicks(true);
        sSupplierB.setShowTickLabels(true);
        sSupplierB.setShowTickMarks(true);
//        sSupplierB.setMinHeight(20); //////////////////////////////////////////////
//        sSupplierB.setPrefHeight(100);//////////////////////////////////////////////
        sSupplierB.valueProperty().addListener(new ChangeListener<Number>() {
            @Override
            public void changed(ObservableValue<? extends Number> changed, Number oldValue, Number newValue) {
                bListenerSpeed.onViewChanged(newValue.longValue());
            }
        });
        Slider sSupplierE = new Slider(500, 10000, timeSupplier);
        sSupplierE.setOrientation(Orientation.HORIZONTAL);
        sSupplierE.setMajorTickUnit(500);
        sSupplierE.setMinorTickCount(0);
        sSupplierE.setSnapToTicks(true);
        sSupplierE.setShowTickLabels(true);
        sSupplierE.setShowTickMarks(true);
        sSupplierE.valueProperty().addListener(new ChangeListener<Number>() {
            @Override
            public void changed(ObservableValue<? extends Number> changed, Number oldValue, Number newValue) {
                eListenerSpeed.onViewChanged(newValue.longValue());
            }
        });
        Slider sSupplierA = new Slider(500, 10000, timeSupplier);
        sSupplierA.setOrientation(Orientation.HORIZONTAL);
        sSupplierA.setMajorTickUnit(500);
        sSupplierA.setMinorTickCount(0);
        sSupplierA.setSnapToTicks(true);
        sSupplierA.setShowTickLabels(true);
        sSupplierA.setShowTickMarks(true);
        sSupplierA.valueProperty().addListener(new ChangeListener<Number>() {
            @Override
            public void changed(ObservableValue<? extends Number> changed, Number oldValue, Number newValue) {
                for (ListenerSpeed l : aListenerSpeed) {
                    l.onViewChanged(newValue.longValue());
                }
            }
        });
        Slider sDealer = new Slider(0, 10000, timeDealer);
        sDealer.setOrientation(Orientation.HORIZONTAL);
        sDealer.setMajorTickUnit(500);
        sDealer.setMinorTickCount(0);
        sDealer.setSnapToTicks(true);
        sDealer.setShowTickLabels(true);
        sDealer.setShowTickMarks(true);
        sDealer.valueProperty().addListener(new ChangeListener<Number>() {
            @Override
            public void changed(ObservableValue<? extends Number> changed, Number oldValue, Number newValue) {
                for (ListenerSpeed l : dListenerSpeed) {
                    l.onViewChanged(newValue.longValue());
                }
            }
        });

        Text bText = new Text("Time sleep for body supplier");
        bText.setStyle("-fx-font-size: 30");
        Text eText = new Text("Time sleep for engine supplier");
        eText.setStyle("-fx-font-size: 30");
        Text aText = new Text("Time sleep for accessory suppliers");
        aText.setStyle("-fx-font-size: 30");
        Text dText = new Text("Time sleep for dealers");
        dText.setStyle("-fx-font-size: 30");

        VBox vBSupplier = new VBox(30, bText, sSupplierB);
        VBox vESupplier = new VBox(30, eText, sSupplierE);
        VBox vASupplier = new VBox(30, aText, sSupplierA);
        VBox vDealer = new VBox(30, dText, sDealer);

        VBox b1 = new VBox(30, tBodyStorage, tEngineStorage, tAccessoryStorage, tCarStorage, tQueueCount);
        VBox b2 = new VBox(130, vBSupplier, vESupplier, vASupplier, vDealer);
//        VBox b2 = new VBox(130, sSupplierB, sSupplierE, sSupplierA, sDealer);
//        VBox.setMargin(sSupplierB, I);
//        b2.setFillWidth(true);
        HBox root = new HBox(30, b1, b2);
        HBox.setHgrow(b2, Priority.ALWAYS);
        //HBox.setHgrow(b1, Priority.NEVER);
        HBox.setMargin(b2, new Insets(30, 30, 30, 30));
        HBox.setMargin(b1, new Insets(30, 30, 30, 30));
        //VBox root = new VBox(130, lBodyStorage, lEngineStorage, lAccessoryStorage, lCarStorage, sSupplierB, sSupplierE, sSupplierA);
        Scene scene = new Scene(root, 2000, 1000);

        stage.setScene(scene);
        //stage.setFullScreen(true);
        stage.setOnCloseRequest(new EventHandler<WindowEvent>() {
            @Override public void handle(WindowEvent t) {
//                serviceSupplier.shutdown();
//                serviceDealer.shutdown();
//                controllerCarStorage.stop();
//                try {
//                    System.out.println(serviceSupplier.awaitTermination(5000, TimeUnit.MILLISECONDS));
//                } catch (InterruptedException e) {
//                    throw new RuntimeException(e);
//                }

//                while (!serviceDealer.isTerminated()) {
//                    System.out.println("!serviceDealer.isTerminated()");
//                }
//                while (!serviceSupplier.isTerminated()) {
//                    System.out.println("!serviceSupplier.isTerminated()");
//                }

                serviceSupplier.shutdownNow();
                serviceDealer.shutdownNow();
                controllerCarStorage.stop();
            }
        });

        stage.show();
    }
    public void parseConfig() {
        String configFile = "src/main/resources/config";
        try (InputStream is = new FileInputStream(configFile)) {
            try (BufferedReader file = new BufferedReader(new InputStreamReader(is))) {
                while (file.ready()) {
                    String[] str = file.readLine().split(" ");
                    if (Objects.equals(str[0], "maxBodyStorage")) {
                        maxBodyStorage = Integer.parseInt(str[2]);
                    }
                    else if (Objects.equals(str[0], "maxEngineStorage")) {
                        maxEngineStorage = Integer.parseInt(str[2]);
                    }
                    else if (Objects.equals(str[0], "maxAccessoryStorage")) {
                        maxAccessoryStorage = Integer.parseInt(str[2]);
                    }
                    else if (Objects.equals(str[0], "maxCarStorage")) {
                        maxCarStorage = Integer.parseInt(str[2]);
                    }
                    else if (Objects.equals(str[0], "S")) {
                        S = Integer.parseInt(str[2]);
                    }
                    else if (Objects.equals(str[0], "D")) {
                        D = Integer.parseInt(str[2]);
                    }
                    else if (Objects.equals(str[0], "W")) {
                        W = Integer.parseInt(str[2]);
                    }

                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    @Override
    public void onModelChanged() {
        Platform.runLater(() -> {
            tBodyStorage.setText("Body storage: " + bodyStorage.getSize() + "/" + maxBodyStorage + " Count: " + bodyStorage.getNumProducedObj());
            tEngineStorage.setText("Engine storage: " + engineStorage.getSize() + "/" + maxEngineStorage + " Count: " + engineStorage.getNumProducedObj());
            tAccessoryStorage.setText("Accessory storage: " + accessoryStorage.getSize() + "/" + maxAccessoryStorage + " Count: " + accessoryStorage.getNumProducedObj());
            tCarStorage.setText("Car storage: " + carStorage.getSize() + "/" + maxCarStorage + " Count: " + carStorage.getNumProducedObj());
            tQueueCount.setText( "Queue: " + carStorage.getTaskCount());
        });
    }
}
