package ru.nsu.testova.view;

import ru.nsu.testova.model.Model;

import javax.swing.SwingUtilities;

public class MainSwing {
    public static void main(String[] args) {
        Model model = new Model();
        SwingUtilities.invokeLater(() -> {
            MainWindowSwing mainWindow = new MainWindowSwing(model);
            mainWindow.setVisible(true);
        });
    }
}