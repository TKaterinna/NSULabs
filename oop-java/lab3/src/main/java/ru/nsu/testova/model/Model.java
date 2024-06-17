package ru.nsu.testova.model;

import ru.nsu.testova.model.objects.GameObject;
import ru.nsu.testova.model.objects.Enemy;
import ru.nsu.testova.model.objects.Name;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.Random;

public class Model implements AutoCloseable {
    private final int playerW = 20;
    private final long timeout = 100;
    private Thread thread;
    private ModelListener listener;
    private GameContext cont;
    //private int addCountEnemies = 10;
    private boolean isStopped = false;

    public Model() {
        //thread = new Ticker(this);
        //cont = new GameContext();
        //generate();
    }

    public synchronized void start() {
        //addCountEnemies = 10;
        thread = new Ticker(this);
        cont = new GameContext();
        isStopped = false;
        generate();
        thread.start();
    }
    public synchronized void startSavedGame() {
        thread = new Ticker(this);
        cont = new GameContext();
        cont.loadGameContext();
        isStopped = false;
        generate();
        thread.start();
    }
    public synchronized void stop() {
        isStopped = true;
        thread.interrupt();
    }
    public synchronized void generate() {
        List<GameObject> gameObjects = cont.getObj();
        for (int i = 0; i < gameObjects.size(); i++) {
            cont.changeCurIndex(i);
            gameObjects.get(i).oneStepChange(cont);
        }
        for (int i = 0; i < gameObjects.size(); ) {
            cont.changeCurIndex(i);
            gameObjects.get(i).catchCollisions(cont);
            if (!gameObjects.get(i).isAlive()) {
                if (i == 0) {
                    cont.setPlayerDead();
                }
                if (gameObjects.get(i).isEnemy()) {
                    cont.increaseCountEnemies(-1);
                }
                gameObjects.remove(i);
            } else {
                i++;
            }
        }
        cont.increaseCountEnemies(makeNewEnemies());
        if (needStop()) {
            stop();
        }
        notifyUnsafe();
    }
    public boolean needStop() {
        return cont.isLoss() || (cont.getAddCountEnemies() == 0 && cont.getCountEnemies() == 0);
    }
    public synchronized void moveLeft() {
        cont.getObj().get(0).moveLeft();
        cont.getObj().get(0).setName(Name.PLAYER_LEFT);
        notifyUnsafe();
    }
    public synchronized void moveRight() {
        cont.getObj().get(0).moveRight();
        cont.getObj().get(0).setName(Name.PLAYER_RIGHT);
        notifyUnsafe();
    }
    public synchronized void makeBullet() {
        cont.makeBullet();
        //cont.getObj().get(0).setName(Name.PLAYER_SHOOTING);
        notifyUnsafe();
    }
    private synchronized int makeNewEnemies() {
        if (cont.getAddCountEnemies() > 0) {
            Random random = new Random();
            if (random.nextInt() % 10 == 0) {
                //int count = rand() % 4;
                int count = 1;
                List<GameObject> gameObjects = cont.getObj();
                for (int i = 0; i < count; i++) {
                    gameObjects.add(new Enemy(random.nextInt(cont.getWmax()- playerW), 4));
                }
                cont.decreaseAddCountEnemies(count);
                return count;
            }
        }
        return 0;
    }
    private void notifyUnsafe() {
        if (null != listener) {
            listener.onModelChanged();
        }
    }
    public long getTimeout() {
        return timeout;
    }
    public synchronized void setListener(ModelListener listener) {
        this.listener = listener;
    }
    @Override
    public void close() throws InterruptedException {
        if (thread != null) {
            thread.interrupt();
            thread.join();
        }
    }
    public GameContext getCont() {
        return cont;
    }
    public boolean isStopped() {
        return isStopped;
    }
    public void saveGame() {
        List<GameObject> objects = cont.getObj();
        try(BufferedWriter writer = new BufferedWriter(new FileWriter("saved_game.txt", false))) {
            writer.write("1\n");
            writer.write(cont.getScore() + "\n" + cont.getCountEnemies() + "\n" + cont.getAddCountEnemies() + "\n");
            for (GameObject object : objects) {
                writer.write(object.save());
                writer.append('\n');
            }
            writer.flush();
        }
        catch(IOException ex) {
            System.out.println(ex.getMessage());
        }
        stop();
        notifyUnsafe();
    }
}
