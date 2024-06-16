package ru.nsu.testova.viewjavafx;

import javafx.application.Platform;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;
import javafx.scene.image.Image;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.text.Text;
import javafx.stage.Stage;
import javafx.event.ActionEvent;
import javafx.stage.WindowEvent;
import ru.nsu.testova.controller.ControllerJavaFX;
import ru.nsu.testova.model.Model;
import ru.nsu.testova.model.ModelListener;
import ru.nsu.testova.model.objects.ColorObject;
import ru.nsu.testova.model.objects.GameObject;
import ru.nsu.testova.model.objects.Name;

import java.io.*;
import java.util.*;

public class MainWindowJavaFX implements ModelListener {
    private final Model model;
    private ControllerJavaFX controller;
    private Canvas canvas;
    private VBox gameRoot;
    private VBox menuRoot;
    private VBox root;
    private GraphicsContext gc;
    private Scene gameScene;
    private Scene menuScene;
    private Scene scene;
    private final Stage stage;
    private Text hpText, enemyCountText, scoreText, saveText;
    private Text endMessage;
    private TextField tfName;
    private HashMap<Name, Image> pictures;

    public MainWindowJavaFX(Model model, Stage stage) {
        this.model = model;
        this.stage = stage;
        stage.setOnCloseRequest(new EventHandler<WindowEvent>() {
            @Override public void handle(WindowEvent t) {
                //model.stop();
                try {
                    model.close();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        });
        stage.setTitle("Space Invaders");
        stage.setResizable(false);
        loadPictures();

        makeMenuScene();
        makeGameScene();

        stage.setScene(menuScene);
        stage.show();
    }

    private void makeMenuScene() {
        Button bNewGame = new Button("start new game");
        bNewGame.setStyle("-fx-font-size: 30");
        bNewGame.setPrefWidth(400);
        bNewGame.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                setGameScene();
            }
        });
        Button bContinueGame = new Button("start saved game");
        bContinueGame.setStyle("-fx-font-size: 30");
        bContinueGame.setPrefWidth(400);
        bContinueGame.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                startSavedGame();
            }
        });
        Button bScoreBoard = new Button("score board");
        bScoreBoard.setStyle("-fx-font-size: 30");
        bScoreBoard.setPrefWidth(400);
        bScoreBoard.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                viewScore();
            }
        });
        menuRoot = new VBox(bNewGame, bContinueGame, bScoreBoard);
        menuRoot.setAlignment(Pos.CENTER);
        menuScene = new Scene(menuRoot, 850, 650);
    }
    private void makeGameScene() {
//        Button bMenu = new Button("return to menu");
//        bMenu.setPrefWidth(200);
//        bMenu.setOnAction(new EventHandler<ActionEvent>() {
//            @Override
//            public void handle(ActionEvent event) {
//                model.stop();
//                //setMenuScene();
//            }
//        });

//        Button bMenu = new Button("Return to menu");
//        bMenu.setStyle("-fx-font-size: 15");
//        bMenu.setOnMouseClicked(e -> { model.stop(); onModelChanged(); });
        saveText = new Text("Press -s- for saving");
        saveText.setStyle("-fx-font-size: 20");
        hpText = new Text("Hp: ");
        hpText.setStyle("-fx-font-size: 20");
        enemyCountText = new Text("Number of enemies: ");
        enemyCountText.setStyle("-fx-font-size: 20");
        scoreText = new Text("Score: ");
        scoreText.setStyle("-fx-font-size: 20");
        //text.setPrefWidth(80);
        canvas = new Canvas(850, 650);
        //canvas.requestFocus();
        gc = canvas.getGraphicsContext2D();
        gameRoot = new VBox(/*bMenu,*/saveText, hpText, enemyCountText, scoreText, canvas);
        gameScene = new Scene(gameRoot, 850, 650);
        model.setListener(this);
        controller = new ControllerJavaFX(model, this);
    }
    @Override
    public void onModelChanged() {
        Platform.runLater(() -> {
            if (!model.isStopped()) {
                gc.clearRect(0, 0, 850, 650);
                //gc.setFill(Color.GRAY);
                //gc.fillRect(0, 0, 850, 650);
                List<GameObject> gameObjects = model.getCont().getObj();
                gc.drawImage(pictures.get(Name.BACKGROUND), 0, 0);
                if (!gameObjects.isEmpty()) {
                    for (GameObject gameObject : gameObjects) {
                        gc.drawImage(pictures.get(gameObject.getName()), gameObject.getX(), gameObject.getY());
                    }
                }
//                if (!gameObjects.isEmpty()) {
//                    for (GameObject gameObject : gameObjects) {
//                        gc.setFill(juxtaposeColors(gameObject.getColor()));
//                        gc.fillRect(gameObject.getX(), gameObject.getY(), gameObject.getW(), gameObject.getH());
//                    }
//                }
                hpText.setText("Hp: " + model.getCont().getHP());
                enemyCountText.setText("Number of enemies: " + model.getCont().getCountEnemies());
                scoreText.setText("Score: " + model.getCont().getScore());
            }
            else {
                setEndMessage();
            }
        });
    }
    public void setGameScene() {
        Platform.runLater(() -> {
//            gamestarted = true;
            stage.setScene(gameScene);
            model.start();
            onModelChanged();
        });
    }
    public void setEndMessage() {
        Platform.runLater(() -> {
            //Stage temp = new Stage();
            if (model.getCont().isLoss()) {
                endMessage = new Text("You are loser!");
            }
            else {
                endMessage = new Text("Not bad.");
            }
            endMessage.setStyle("-fx-font-size: 50");

            Text tName = new Text("Your name:");
            tName.setStyle("-fx-font-size: 30");
            tfName = new TextField();
            tfName.setStyle("-fx-font-size: 30");
            Button bSaveScore = new Button("Save score");
            bSaveScore.setStyle("-fx-font-size: 30");
            bSaveScore.setOnMouseClicked(e -> { saveScore(); });

            root = new VBox(endMessage, tName, tfName, bSaveScore);
            root.setAlignment(Pos.CENTER);
            scene = new Scene(root, 850, 650);
            stage.setScene(scene);
            stage.show();
        });
    }
    public void viewScore() {
        try(BufferedReader br = new BufferedReader(new FileReader("score.txt"))) {
            String s;
            StringBuilder text = new StringBuilder();
            while((s=br.readLine()) != null) {
                text.append(s);
                text.append("\n");
            }
            Text tScore = new Text(text.toString());
            tScore.setStyle("-fx-font-size: 30");
            Button bMenu = new Button("Menu");
            bMenu.setStyle("-fx-font-size: 30");
            bMenu.setOnMouseClicked(e -> { setMenuScene(); });
            stage.setScene(new Scene(new VBox(bMenu, tScore), 850, 650));
        }
        catch(IOException ex) {
            System.out.println(ex.getMessage());
        }
    }
    public static class Score implements Comparable<Score> {
        String name;
        int score;
        public Score (String name, int score) {
            this.name = name;
            this.score = score;
        }

        @Override
        public int compareTo(Score another) {

            return Integer.compare(another.score, this.score);
        }

        public int getScore() {
            return score;
        }

        public String getName() {
            return name;
        }
    }
    public void saveScore() {

        List<Score> scores = new ArrayList<>();

        try(BufferedReader br = new BufferedReader(new FileReader("score.txt"))) {
            String s;
            String[] strings;
            while((s=br.readLine()) != null) {
                strings = s.split(" ");
                scores.add(new Score(strings[0], Integer.parseInt(strings[1])));
            }
        }
        catch(IOException ex) {
            System.out.println(ex.getMessage());
        }
        scores.add(new Score(tfName.getText(), model.getCont().getScore()));

        Collections.sort(scores);

        try(BufferedWriter writer = new BufferedWriter(new FileWriter("score.txt", false))) {
            for (Score object : scores){
                String text = object.getName() + " " + object.getScore();
                writer.write(text);
                writer.append('\n');
            }
            writer.flush();
        }
        catch(IOException ex) {
            System.out.println(ex.getMessage());
        }

//        try(BufferedWriter writer = new BufferedWriter(new FileWriter("score.txt", true))) {
//            String text = tfName.getText() + " " + model.getCont().getScore();
//            writer.write(text);
//            writer.append('\n');
//            writer.flush();
//        }
//        catch(IOException ex) {
//            System.out.println(ex.getMessage());
//        }
        setMenuScene();
    }
    public void setMenuScene() {
        Platform.runLater(() -> {
            stage.setScene(menuScene);
            //stage.show();
        });
    }
    public void loadPictures() {
        pictures = new HashMap<>();
        for (Name n : Name.values()) { pictures.put(n,
          new Image(Objects.requireNonNull(getClass().getResourceAsStream("/ru/nsu/testova/pictures/" + n.toString().toLowerCase() + ".png"))));}
    }
    private Paint juxtaposeColors(ColorObject modelObject) {
        switch (modelObject) {
            case RED -> {return Color.RED;}
            case BLACK -> {return Color.BLACK;}
            case BLUE -> {return Color.BLUE;}
            case GREEN -> {return Color.GREEN;}
        }
        return Color.BLACK;
    }
    public Scene getGameScene() {
        return gameScene;
    }
    public void startSavedGame() {
        Platform.runLater(() -> {
            stage.setScene(gameScene);
            model.startSavedGame();
            onModelChanged();
        });
    }
}
