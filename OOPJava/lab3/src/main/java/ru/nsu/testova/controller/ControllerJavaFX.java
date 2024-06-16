package ru.nsu.testova.controller;

import ru.nsu.testova.model.Model;
import ru.nsu.testova.viewjavafx.MainWindowJavaFX;

import javafx.scene.input.KeyEvent;

public class ControllerJavaFX  {
    private Model model;
    private MainWindowJavaFX window;
    public ControllerJavaFX(Model model, MainWindowJavaFX window) {
        this.model = model;
        this.window = window;
        KeyListener();
    }

    public void KeyListener() {
        window.getGameScene().setOnKeyPressed(
            (KeyEvent event) -> {
                switch (event.getCode()) {
                    case LEFT -> model.moveLeft();
                    case RIGHT -> model.moveRight();
                    case SPACE -> model.makeBullet();
                    case S -> model.saveGame();
                }
            }
        );
    }
}
