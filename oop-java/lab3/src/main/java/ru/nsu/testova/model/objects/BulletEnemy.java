package ru.nsu.testova.model.objects;

import ru.nsu.testova.model.GameContext;

public class BulletEnemy extends Bullet {
    public BulletEnemy(int x, int y) {
        super (x, y, ColorObject.BLACK, Name.ENEMY_BULLET);
    }
    public BulletEnemy(int x, int y, int hp) {
        super (x, y, hp, ColorObject.GREEN, Name.PLAYER_BULLET);
    }
    public void oneStepChange(GameContext cont) {
        moveDown();
        if (isOutOfScreen(cont)) {
            hp = 0;
        }
    }
    public String save() {
        return "enemy_bullet " + x + " " + y + " " + hp;
    }
}
