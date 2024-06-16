package ru.nsu.testova.messages;

import java.util.List;

public class ListMembers implements IMessage {
    private TypeMessage type;
    private List<String> members;
    public ListMembers(List<String> members) {
        this.members = members;
        type = TypeMessage.LIST;
    }
    @Override
    public TypeMessage getType() {
        return type;
    }

    @Override
    public String getMassage() {
        StringBuilder stringBuilder = new StringBuilder();

        for (String m : members) {
            stringBuilder.append(m).append('\n');
        }
        return stringBuilder.toString();
    }
}
