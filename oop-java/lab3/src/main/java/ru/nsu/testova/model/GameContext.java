package ru.nsu.testova.model;

import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;
import ru.nsu.testova.model.objects.*;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class GameContext {
    private final List<GameObject> obj;
    private int curIndex;
    private int countEnemies;
    private int score = 0;
    private int hmax = 500;
    private int wmax = 600;
    private boolean playerShooting = false;
    private boolean isLoss = false;
    private int addCountEnemies = 10;

    public GameContext() {
        obj = new ArrayList<GameObject>();
        obj.add(new Player((wmax - 30) / 2 , hmax - 1 - 30));
        addCountEnemies = 10;
    }

    public void loadGameContext() {
        try(BufferedReader br = new BufferedReader(new FileReader("saved_game.txt"))) {
            obj.clear();
            String s;
            String[] objectInf;
            if (Objects.equals(br.readLine(), "1")) {
                score = Integer.parseInt(br.readLine());
                countEnemies = Integer.parseInt(br.readLine());
                addCountEnemies = Integer.parseInt(br.readLine());
                while ((s = br.readLine()) != null) {
                    objectInf = s.split(" ");
                    if (Objects.equals(objectInf[0], "player")) {
                        obj.add(new Player(Integer.parseInt(objectInf[1]), Integer.parseInt(objectInf[2]), Integer.parseInt(objectInf[3])));
                    }
                    else if (Objects.equals(objectInf[0], "enemy")) {
                        obj.add(new Enemy(Integer.parseInt(objectInf[1]), Integer.parseInt(objectInf[2]), Integer.parseInt(objectInf[3])));
                    }
                    else if (Objects.equals(objectInf[0], "player_bullet")) {
                        obj.add(new BulletPlayer(Integer.parseInt(objectInf[1]), Integer.parseInt(objectInf[2]), Integer.parseInt(objectInf[3])));
                    }
                    else if (Objects.equals(objectInf[0], "enemy_bullet")) {
                        obj.add(new BulletEnemy(Integer.parseInt(objectInf[1]), Integer.parseInt(objectInf[2]), Integer.parseInt(objectInf[3])));
                    }
                }
            }
        }
        catch(IOException ex) {
            System.out.println(ex.getMessage());
        }
    }
    public int getAddCountEnemies() {
        return addCountEnemies;
    }
    public void decreaseAddCountEnemies(int i) {
        addCountEnemies -= i;
    }
    public List<GameObject> getObj() {
        return obj;
    }
    public void changeCurIndex(int i) {
        curIndex = i;
    }
    public void increaseCountEnemies(int i) {
        countEnemies += i;
    }
    public void increaseScore(int i) { score += i; }
    public int getHmax() {
        return hmax;
    }
    public int getWmax() {
        return wmax;
    }
    public int getCurIndex() {
        return curIndex;
    }
    public void makeBullet() {
        playerShooting = true;
    }
    public boolean isPlayerShooting() {
        return playerShooting;
    }
    public void setPlayerNoShooting() {
        playerShooting = false;
    }
    public int getScore() {
        return score;
    }
    public int getCountEnemies() {
        return countEnemies;
    }
    public void setPlayerDead() {
        isLoss = true;
    }
    public boolean isLoss() {
        return isLoss;
    }
    public int getHP() {
        return obj.get(0).getHP();
    }
}
