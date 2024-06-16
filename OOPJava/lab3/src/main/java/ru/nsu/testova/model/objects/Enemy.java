package ru.nsu.testova.model.objects;

import ru.nsu.testova.model.GameContext;

public class Enemy extends GameObject {
    private long prevBulletTimeMake;
    public Enemy(int x, int y) {
        super (x, y, 3, 1, 34, 30, ColorObject.RED, Name.ENEMY);
        prevBulletTimeMake = getTime();
    }
    public Enemy(int x, int y, int hp) {
        super (x, y, 3, hp, 34, 30, ColorObject.RED, Name.ENEMY);
        prevBulletTimeMake = getTime();
    }
    public void oneStepChange(GameContext cont) {
        moveDown();
        if (isOutOfScreen(cont)) {
            hp = 0;
            cont.getObj().get(0).hurt(1);
        }
        if (readyShoot()) {
            cont.getObj().add(new BulletEnemy(x + w / 2, y + 30));
            prevBulletTimeMake = getTime();
        }
    }
    public boolean isEnemy() {
        return true;
    }
    public boolean isBullet() {
        return false;
    }
    public long getTime() {
        return System.currentTimeMillis();
    }
    public boolean readyShoot() {
        return (getTime() - prevBulletTimeMake) >= 2000; //2 сек
    }
    public String save() {
        return "enemy " + x + " " + y + " " + hp;
    }
}