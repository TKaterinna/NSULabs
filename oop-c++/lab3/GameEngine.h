#pragma once
#include <vector>
#include "Objects.h"

namespace game_engine {
    class GameEngine {
    private:
        game_context_objects::GameContext& cont;
        int add_count_enemies;
    public:
        GameEngine(game_context_objects::GameContext& context, int count_enemies)
        : cont{context}, add_count_enemies{count_enemies} {};
        ~GameEngine() = default;

        std::vector<std::unique_ptr <game_context_objects::DynamicObject>>& get_obj() {
            return cont.get_game_objects();
        };
        void reprint();
        void print_map(const char* title_format_str);
        bool need_to_stop() { return cont.get_player_is_dead() || (add_count_enemies == 0 && get_count_enemies() == 0); };
        int get_count_enemies() {return cont.get_count_enemies(); };
        int get_add_count_enemies() {return add_count_enemies; };
        int make_new_enemies();
    };
}