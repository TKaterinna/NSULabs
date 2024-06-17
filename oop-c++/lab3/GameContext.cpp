#include "Objects.h"

namespace game_context_objects {
    void GameContext::get_input(int c) {
        pressed = false;
        switch (c) {
            case KEY_LEFT:
                dest = "left";
                pressed = true;
                break;
            case KEY_RIGHT:
                dest = "right";
                pressed = true;
                break;
            case ' ':
                dest = "bullet";
                pressed = true;
                break;
        }
    }

    std::vector<std::unique_ptr <DynamicObject>>& GameContext::get_game_objects() {
        return obj;
    }

    bool GameContext::get_pressed() {
        return pressed;
    }

    std::string& GameContext::get_dest() {
        return dest;
    }

    void GameContext::change_count_enemies(int param) {
            count_enemies += param;
    }
}