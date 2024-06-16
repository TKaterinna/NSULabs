package ru.nsu.testova.model.objects;

import ru.nsu.testova.model.GameContext;

public abstract class Bullet extends GameObject{
//    public Bullet(int x, int y) {
//        super (x - 5, y, 15, 1, 20, 20, ColorObject.BLACK, Name.PLAYER_BULLET);
//    }
    public Bullet(int x, int y, ColorObject color, Name name) {
        super (x - 5, y, 15, 1, 20, 20, color, name);
    }
    public Bullet(int x, int y, int hp, ColorObject color, Name name) {
        super (x - 5, y, 15, hp, 20, 20, color, name);
    }

    public boolean isEnemy() {
        return false;
    }
    public boolean isBullet() {
        return true;
    }
    public abstract void oneStepChange(GameContext cont);
    public String save() {
        return "player_bullet " + x + " " + y + " " + hp;
    }
}