package ru.nsu.testova.model.objects;

import ru.nsu.testova.model.GameContext;

import java.util.List;

public class Player extends GameObject {
    public Player(int x, int y) {
        super (x, y, 10, 3, 64, 40, ColorObject.BLUE, Name.PLAYER);
    }
    public Player(int x, int y, int hp) {
        super (x, y, 10, hp, 64, 40, ColorObject.BLUE, Name.PLAYER);
    }
    public void oneStepChange(GameContext cont) {
        name = Name.PLAYER;
        if (cont.isPlayerShooting()) {
            List<GameObject> obj = cont.getObj();
            obj.add(new BulletPlayer(obj.get(0).getX() + obj.get(0).getW() / 2, obj.get(0).getY() - 20));
            cont.setPlayerNoShooting();
        }
    }
    public boolean isEnemy() {
        return false;
    }
    public boolean isBullet() {
        return false;
    }
    public String save() {
        return "player " + x + " " + y + " " + hp ;
    }
}
