package ru.nsu.testova.view;

import ru.nsu.testova.controller.ButtonController;
import ru.nsu.testova.controller.ControllerSwing;
import ru.nsu.testova.model.Model;
import ru.nsu.testova.model.ModelListener;
import ru.nsu.testova.model.objects.ColorObject;
import ru.nsu.testova.model.objects.GameObject;
import ru.nsu.testova.viewjavafx.MainWindowJavaFX;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.text.Document;
import javax.swing.text.PlainDocument;
import java.awt.*;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.List;

public class MainWindowSwing extends JFrame implements ModelListener {
    private final Model model;
    private DrawGamePanel gameScreen;
    private final DrawMenuPanel menuScreen;
    private EndMessagePanel endMessageScreen;
    private ScorePanel scoreScreen;
    private final ButtonController buttonController;
    private ControllerSwing controller;
    private final Document inputModel;
    private static String hp, enemies, score;

    public MainWindowSwing(Model model) {
        super("Swing Space Invaders");
        setResizable(false);
        this.model = model;
        inputModel = new PlainDocument();
        buttonController = new ButtonController(this, model);

        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        addWindowListener(buttonController);
        menuScreen = new DrawMenuPanel(buttonController);

        setSize(850, 700);
        //setExtendedState(JFrame.MAXIMIZED_BOTH);
        add(menuScreen);
        setLocationRelativeTo(null);
        setVisible(true);
        model.setListener(this);
        //onModelChanged();
    }

    public Document getInputModel() {
        return inputModel;
    }
    @Override
    public void onModelChanged() {
        SwingUtilities.invokeLater(() -> {
            if (!model.isStopped()) {
                hp = Integer.toString(model.getCont().getHP());
                enemies = Integer.toString(model.getCont().getCountEnemies());
                score = Integer.toString(model.getCont().getScore());
                repaint();
            }
            else {
                setEndMessage();
            }
        });
    }
    public void setEndMessage() {
        remove(gameScreen);
        String endMessage;
        if (model.getCont().isLoss()) {
            endMessage = "You are loser!";
        }
        else {
            endMessage = "Not bad.";
        }
        endMessageScreen = new EndMessagePanel(buttonController, controller, endMessage);
        add(endMessageScreen);
        removeWindowListener(controller);
        removeKeyListener(controller);
        addWindowListener(buttonController);
        revalidate();
        repaint();
        requestFocusInWindow();
    }
    public void changeEndMessageToMenu() {
        SwingUtilities.invokeLater(() -> {
            remove(endMessageScreen);
            add(menuScreen);
            revalidate();
            repaint();
            requestFocusInWindow();
        });
    }
    public void changeMenuToGame() {
        SwingUtilities.invokeLater(() -> {
            remove(menuScreen);
            model.start();
            gameScreen = new DrawGamePanel(model.getCont().getObj());
            add(gameScreen);
            controller = new ControllerSwing(model, inputModel);
            removeWindowListener(buttonController);
            addWindowListener(controller);
            addKeyListener(controller);
            revalidate();
            repaint();
            requestFocusInWindow();
        });
    }
    public void changeMenuToScore() {
        SwingUtilities.invokeLater(() -> {
            remove(menuScreen);
            add(scoreScreen);
            //removeWindowListener(buttonController);
            //addWindowListener(controller);
            //addKeyListener(controller);
            revalidate();
            repaint();
            requestFocusInWindow();
        });
    }
    public void changeScoreToMenu() {
        SwingUtilities.invokeLater(() -> {
            remove(scoreScreen);
            add(menuScreen);
            //removeWindowListener(buttonController);
            //addWindowListener(controller);
            //addKeyListener(controller);
            revalidate();
            repaint();
            requestFocusInWindow();
        });
    }
    public void viewScore() {
        try(BufferedReader br = new BufferedReader(new FileReader("score.txt"))) {
            String s;
            StringBuilder text = new StringBuilder();
            while((s=br.readLine()) != null) {
                text.append(s);
                //text.append("\n");
                text.append("<br>");
            }
            scoreScreen = new ScorePanel(buttonController, text.toString());
//            add(scoreScreen);
            //bMenu.setOnMouseClicked(e -> { setMenuScene(); });
            //stage.setScene(new Scene(new VBox(bMenu, tScore), 850, 650));
        }
        catch(IOException ex) {
            System.out.println(ex.getMessage());
        }
    }
    public static class DrawMenuPanel extends JPanel {
        private static final int PADDING = 1000;
        public DrawMenuPanel(ButtonController controller) {
            Font BigFontTR = new Font("TimesRoman", Font.BOLD, 30);

            JButton newGame = new JButton("start new game");
            newGame.setFont(BigFontTR);
            newGame.setBackground(Color.cyan);
            newGame.setBorder(new EmptyBorder(10, 30, 10, 30));
            addHorizontalPadding(this);
            newGame.setActionCommand(ButtonController.newGame);
            newGame.addActionListener(controller);
            add(newGame, BorderLayout.CENTER);

//            JButton continueGame = new JButton("start saved game");
//            continueGame.setFont(BigFontTR);
//            continueGame.setBackground(Color.cyan);
//            continueGame.setBorder(new EmptyBorder(10, 19, 10, 19));
//            addHorizontalPadding(this);
//            continueGame.setActionCommand(ButtonController.savedGame);
//            continueGame.addActionListener(controller);
//            add(continueGame, BorderLayout.CENTER);

            JButton scoreBoard = new JButton("score board");
            scoreBoard.setFont(BigFontTR);
            scoreBoard.setBackground(Color.cyan);
            scoreBoard.setBorder(new EmptyBorder(10, 61, 10, 61));
            addHorizontalPadding(this);
            scoreBoard.setActionCommand(ButtonController.scoreBoard);
            scoreBoard.addActionListener(controller);
            add(scoreBoard, BorderLayout.CENTER);
        }
        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            revalidate();
        }

        @Override
        public Dimension getPreferredSize() {
            return new Dimension(850, 650);
        }

        private void addHorizontalPadding(Container container) {
            container.add(Box.createRigidArea(new Dimension(PADDING, 0)));
        }
    }
    public static class DrawGamePanel extends JPanel {
        private final List<GameObject> gameObjects;
        private final JLabel hpText, scoreText, enemyCountText;
        public DrawGamePanel(List<GameObject> obj) {
            gameObjects = obj;

            Font BigFontTR = new Font("TimesRoman", Font.BOLD, 20);

            hpText = new JLabel("Hp: ");
            hpText.setFont(BigFontTR);
            add(hpText);
            enemyCountText = new JLabel("Number of enemies: ");
            enemyCountText.setFont(BigFontTR);
            add(enemyCountText);
            scoreText = new JLabel("Score: ");
            scoreText.setFont(BigFontTR);
            add(scoreText);
        }
        public void rewriteInf(String hp, String enemies, String score) {
            hpText.setText("Hp: " + hp);
            enemyCountText.setText("Number of enemies: " + enemies);
            scoreText.setText("Score: " + score);
        }
        @Override
        protected void paintComponent(Graphics g) {
            rewriteInf(hp, enemies, score);
            super.paintComponent(g);

            if (!gameObjects.isEmpty()) {
                for (GameObject gameObject : gameObjects) {
                    g.setColor(juxtaposeColors(gameObject.getColor()));
                    g.fillRect(gameObject.getX(), gameObject.getY() + 50, gameObject.getW(), gameObject.getH());
                }
            }
        }

        private Color juxtaposeColors(ColorObject modelObject) {
            switch (modelObject) {
                case RED -> {return Color.RED;}
                case BLACK -> {return Color.BLACK;}
                case BLUE -> {return Color.BLUE;}
                case GREEN -> {return Color.GREEN;}
            }
            return Color.BLACK;
        }
        @Override
        public Dimension getPreferredSize() {
            return new Dimension(850, 700);
        }
    }
    public static class EndMessagePanel extends JPanel {
        private static final int PADDING = 1000;
        public EndMessagePanel(ButtonController buttonController, ControllerSwing controller, String endMessage) {
            Font BigFontTR = new Font("TimesRoman", Font.BOLD, 30);

            JLabel label = new JLabel(endMessage);
            label.setFont(BigFontTR);
            add(label, BorderLayout.CENTER);
            addHorizontalPadding(this);

            JTextField tfName = new JTextField(controller.getInputModel(), "", 6);
            add(tfName, BorderLayout.CENTER);
            addHorizontalPadding(this);

            JButton bSaveScore = new JButton("Save score");
            bSaveScore.setFont(BigFontTR);
            bSaveScore.setBackground(Color.cyan);
            bSaveScore.setBorder(new EmptyBorder(10, 30, 10, 30));
            bSaveScore.setActionCommand(ButtonController.saveScore);
            bSaveScore.addActionListener(buttonController);
            add(bSaveScore, BorderLayout.CENTER);
        }
        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            revalidate();
        }

        @Override
        public Dimension getPreferredSize() {
            return new Dimension(850, 650);
        }

        private void addHorizontalPadding(Container container) {
            container.add(Box.createRigidArea(new Dimension(PADDING, 0)));
        }
    }
    public static class ScorePanel extends JPanel {
        private static final int PADDING = 1000;
        public ScorePanel(ButtonController buttonController, String text) {
            Font BigFontTR = new Font("TimesRoman", Font.BOLD, 30);
            JButton bMenu = new JButton("Menu");
            bMenu.setFont(BigFontTR);
            bMenu.setActionCommand(ButtonController.backToMenu);
            bMenu.addActionListener(buttonController);
            add(bMenu, BorderLayout.CENTER);
            addHorizontalPadding(this);
            JLabel tScore = new JLabel("<html>" + text + "</html>");
            tScore.setFont(BigFontTR);
            add(tScore);
        }
        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            revalidate();
        }
        @Override
        public Dimension getPreferredSize() {
            return new Dimension(850, 650);
        }
        private void addHorizontalPadding(Container container) {
            container.add(Box.createRigidArea(new Dimension(PADDING, 0)));
        }
    }
}
