package ru.nsu.testova.model.objects;

import ru.nsu.testova.model.GameContext;

public class BulletPlayer extends Bullet {
    public BulletPlayer(int x, int y) {
        super (x, y, ColorObject.GREEN, Name.PLAYER_BULLET);
    }
    public BulletPlayer(int x, int y, int hp) {
        super (x, y, hp, ColorObject.GREEN, Name.PLAYER_BULLET);
    }
    public void oneStepChange(GameContext cont) {
        moveUp();
        if (isOutOfScreen(cont)) {
            hp = 0;
        }
    }
    public String save() {
        return "player_bullet " + x + " " + y + " " + hp;
    }
}
