#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
//#include <SFML/Audio.hpp>

#include <chrono>
#include <vector>
#include <algorithm>

#include "Objects.h"
#include "GameEngine.h"
//#include "Printer.h"
namespace game {
    struct ScoreLine;
    class Game {
    private:
        int hmax = 0;
        int wmax = 0;
        void print_menu();
        void print_score_bord(std::vector<ScoreLine>& records);
        void get_score_board(std::vector<ScoreLine>& records);
        void save_score(ScoreLine& cur, std::vector<ScoreLine>& records);
        void playing(game_context_objects::GameContext& context, game_engine::GameEngine& eng);
        void save_game(game_engine::GameEngine& eng, game_context_objects::GameContext& cont);
    public:
        void start_game();
    };
}