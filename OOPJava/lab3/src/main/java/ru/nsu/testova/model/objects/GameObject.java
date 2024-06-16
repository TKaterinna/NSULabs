package ru.nsu.testova.model.objects;

import ru.nsu.testova.model.GameContext;

import java.util.List;

public abstract class GameObject implements IGameObject {
    protected ColorObject color;
    protected int hp = 1;
    protected int x = 0;
    protected int y = 0;
    protected int speed = 1;
    protected int h = 30;
    protected int w = 30;
    protected Name name;

    public GameObject(int x, int y, int speed, int hp, int h, int w, ColorObject color, Name name) {
        this.x = x;
        this.y = y;
        this.speed = speed;
        this.hp = hp;
        this.h = h;
        this.w = w;
        this.color = color;
        this.name = name;
    }

    public Name getName() {
        return name;
    }
    public void setName(Name name) {
        this.name = name;
    }
    public abstract void oneStepChange(GameContext cont);
    public abstract boolean isEnemy();
    public abstract boolean isBullet();
    protected void hurt(int k) {
        hp -= k;
    }
    protected boolean isOutOfScreen(GameContext cont) {
        return (y > cont.getHmax() || y < 0);
    }
    public boolean isAlive() {
        return (hp > 0);
    }
    public void catchCollisions(GameContext cont) {
        List<GameObject> gameObjects = cont.getObj();
        for (int i = cont.getCurIndex() + 1; i < gameObjects.size(); i++) {
            //if (i != cont.getCurIndex()) {
                GameObject obj2 = gameObjects.get(i);
                if ((this.x + this.w >= obj2.x) && (this.x <= obj2.x + obj2.w) &&
                        (this.y + this.h >= obj2.y) && (this.y <= obj2.y + obj2.h)) {
                    obj2.hurt(1);
                    this.hurt(1);
                    if ((obj2.isEnemy() && this.isBullet()) || (obj2.isBullet() && this.isEnemy())) {
                        cont.increaseScore(1);
                    }
                    break;
                }
            //}
        }
    }
    public int getX() {
        return x;
    }
    public int getY() {
        return y;
    }
    public int getH() {
        return h;
    }
    public int getW() {
        return w;
    }
    public int getHP() { return hp; }
    public void moveLeft() {
        x -= speed;
    }
    public void moveRight() { x += speed; }
    public void moveUp() { y -= speed; }
    public void moveDown() { y += speed; }
    public ColorObject getColor() {
        return color;
    }
}
