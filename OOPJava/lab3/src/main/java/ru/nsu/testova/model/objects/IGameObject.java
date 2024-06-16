package ru.nsu.testova.model.objects;

import ru.nsu.testova.model.GameContext;

public interface IGameObject {
    void oneStepChange(GameContext cont);
    boolean isAlive();
    void catchCollisions(GameContext cont);
    void moveLeft();
    void moveRight();
    String save();
}
