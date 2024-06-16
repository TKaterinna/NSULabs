package ru.nsu.testova;

import ru.nsu.testova.exeptions.CalcExceptions;
import ru.nsu.testova.exeptions.SystemExceptions;

import java.util.HashMap;
import java.util.Map;

public class ProxyFactory implements CommandFactory {
    private final CommandFactory factory;
    private final Map<String, Command> mapCommands = new HashMap<>();
    ProxyFactory(CommandFactory f) {
        factory = f;
    }
    public Command create(String command) throws CalcExceptions, SystemExceptions {
        try {
            if (mapCommands.containsKey(command)) {
                return mapCommands.get(command);
            } else {
                mapCommands.put(command, factory.create(command));
                return mapCommands.get(command);
            }
        }
        catch (Exception ex) {
            throw new SystemExceptions(ex);
        }
    }

}
