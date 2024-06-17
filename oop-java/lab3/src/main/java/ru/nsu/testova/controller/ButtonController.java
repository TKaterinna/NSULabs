package ru.nsu.testova.controller;

import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;
import ru.nsu.testova.model.Model;
import ru.nsu.testova.view.MainWindowSwing;
import ru.nsu.testova.viewjavafx.MainWindowJavaFX;

import javax.swing.*;
import javax.swing.text.BadLocationException;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.*;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;

public class ButtonController extends WindowAdapter implements ActionListener {
    private final MainWindowSwing window;
    private final Model model;
    public static final String newGame = "newGame";
    //public static final String savedGame = "savedGame";
    public static final String scoreBoard = "scoreBoard";
    public static final String saveScore = "saveScore";
    public static final String backToMenu = "backToMenu";
    public ButtonController(MainWindowSwing window, Model model) {
        this.window = window;
        this.model = model;
    }
    public static class StScore implements Comparable<StScore> {
        String name;
        int score;
        public StScore (String name, int score) {
            this.name = name;
            this.score = score;
        }
        @Override
        public int compareTo(StScore another) {

            return Integer.compare(another.score, this.score);
        }
        public int getScore() {
            return score;
        }
        public String getName() {
            return name;
        }
    }
    @Override
    public void actionPerformed(ActionEvent e) {
        if (Objects.equals(e.getActionCommand(), newGame)) {
            window.changeMenuToGame();
        }
        else if (Objects.equals(e.getActionCommand(), scoreBoard)) {
            window.viewScore();
            window.changeMenuToScore();
        }
        else if (Objects.equals(e.getActionCommand(), saveScore)) {
            saveScore();
            window.changeEndMessageToMenu();
        }
        else if (Objects.equals(e.getActionCommand(), backToMenu)) {
            window.changeScoreToMenu();
        }
    }
    public void saveScore() {
        List<StScore> scores = new ArrayList<>();

        try(BufferedReader br = new BufferedReader(new FileReader("score.txt"))) {
            String s;
            String[] strings;
            while((s=br.readLine()) != null) {
                strings = s.split(" ");
                scores.add(new StScore(strings[0], Integer.parseInt(strings[1])));
            }
        }
        catch(IOException ex) {
            System.out.println(ex.getMessage());
        }
        try {
            scores.add(new StScore(window.getInputModel().getText(0, window.getInputModel().getLength()), model.getCont().getScore()));
        } catch (BadLocationException e) {
            throw new RuntimeException(e);
        }

        Collections.sort(scores);

        try(BufferedWriter writer = new BufferedWriter(new FileWriter("score.txt", false))) {
            for (StScore object : scores){
                String text = object.getName() + " " + object.getScore();
                writer.write(text);
                writer.append('\n');
            }
            writer.flush();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

//        try(BufferedWriter writer = new BufferedWriter(new FileWriter("score.txt", true))) {
//            String text = window.getInputModel().getText(0, window.getInputModel().getLength()) + " " + model.getCont().getScore();
//            writer.write(text);
//            writer.append('\n');
//            writer.flush();
//        }
//        catch(IOException ex) {
//            System.out.println(ex.getMessage());
//        } catch (BadLocationException e) {
//            throw new RuntimeException(e);
//        }
    }
    @Override
    public void windowClosing(WindowEvent e) {
        e.getWindow().setVisible(false);
        e.getWindow().dispose();
    }
}
