package ru.nsu.testova.controller;

import ru.nsu.testova.model.Model;

import javax.swing.text.Document;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public class ControllerSwing extends WindowAdapter implements KeyListener {
    private final Model model;
    private Document inputModel;
    public ControllerSwing(Model model, Document inputModel) {
        this.model = model;
        this.inputModel = inputModel;
    }

    public Document getInputModel() {
        return inputModel;
    }

    @Override
    public void windowClosing(WindowEvent e) {
        try {
            model.close();
        } catch (InterruptedException ex) {
            throw new RuntimeException(ex);
        }
        e.getWindow().setVisible(false);
        e.getWindow().dispose();
    }

    @Override
    public void keyTyped(KeyEvent keyEvent) {

    }

    @Override
    public void keyPressed(KeyEvent keyEvent) {
        int keyCode = keyEvent.getKeyCode();
        if (keyCode == KeyEvent.VK_LEFT) {
            model.moveLeft();
        }
        else if (keyCode == KeyEvent.VK_RIGHT) {
            model.moveRight();
        }
        else if (keyCode == KeyEvent.VK_SPACE) {
            model.makeBullet();
        }
    }

    @Override
    public void keyReleased(KeyEvent keyEvent) {

    }
}