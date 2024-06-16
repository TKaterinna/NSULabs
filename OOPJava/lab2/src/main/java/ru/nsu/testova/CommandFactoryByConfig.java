package ru.nsu.testova;

import ru.nsu.testova.exeptions.CalcExceptions;
import ru.nsu.testova.exeptions.IncorrectCommand;
import ru.nsu.testova.exeptions.SystemExceptions;


import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Objects;

public class CommandFactoryByConfig implements CommandFactory {
    private final String configFile;

    CommandFactoryByConfig() {
        configFile = "src/main/resources/config.txt";
    }
    CommandFactoryByConfig(String fileName) {
        configFile = fileName;
    }
    public Command create(String command) throws CalcExceptions, SystemExceptions {
        try {
            try (InputStream is = new FileInputStream(configFile)) {
                try (BufferedReader file = new BufferedReader(new InputStreamReader(is))) {
                    while (file.ready()) {
                        String[] str = file.readLine().split(" ");
                        if (Objects.equals(str[0], command)) {
                            Class<?> c = Class.forName(str[1]);
                            Command com = null;
                            com = (Command) c.getDeclaredConstructor().newInstance();
                            return com;
                        }
                    }
                }
            }
            throw new IncorrectCommand(command);
        }
        catch (Exception ex) {
            throw new SystemExceptions(ex);
        }
    }
}
