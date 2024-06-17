#include "Game.h"
namespace game {
    struct ScoreLine {
        std::string name;
        int score;
    };

    struct ObjInf {
        std::string type;
        int x;
        int y;
        int hp;
    };

    void Game::start_game() {
        setlocale(LC_ALL, "");
        // init curses
        initscr();
        // disable buffering
        cbreak();
        // disable input character echo
        noecho();
        // disable cursor
        curs_set(FALSE);
        // enable function keys as single character values, like KEY_LEFT, etc.
        keypad(stdscr, TRUE);
        // set a very small delay in milliseconds for getch/wgetch to wait for input. Without it, getch/wgetch would block.
        timeout(20);
        //printer::Printer pr();
        getmaxyx(stdscr, hmax, wmax);

        int c = 0;
        std::vector<ScoreLine> records;
        get_score_board(records);
        do {
            print_menu();
            refresh();
            if (c == 'p') {
                game_context_objects::GameContext context(hmax, wmax);
                int level = 1;
                int count_enemies = level * 20;
                game_engine::GameEngine eng(context, count_enemies);
                playing(context, eng);
            } else if (c == 's') {
                while ('c' != (c = getch())) {
                    print_score_bord(records);
                }
            }
            else if (c == 'w') {
                std::ifstream file;
                file.open("Saved_game.txt");

                std::string line;
                std::getline(file, line);
                if (line != "1") {
                    continue; ///////////////////////
                }
                std::getline(file, line);
                std::istringstream str_cont(line);
                int sc;
                str_cont >> sc;
                int c_en;
                str_cont >> c_en;
                game_context_objects::GameContext context(hmax, wmax, sc, c_en);
                int c_add_en;
                str_cont >> c_add_en;
                game_engine::GameEngine eng(context, c_add_en);

                std::vector<std::unique_ptr<game_context_objects::DynamicObject>>& obj =context.get_game_objects();
                ObjInf object;
                while (std::getline(file, line)) {
                    std::istringstream str_line(line);
                    str_line >> object.type;
                    str_line >> object.x;
                    str_line >> object.y;
                    str_line >> object.hp;
                    obj.push_back(game_context_objects::create_object(object.type, object.x, object.y, object.hp));
                }
                playing(context, eng);
            }
        } while ('\n' != (c = getch()));
        endwin();
    }

    void Game::playing(game_context_objects::GameContext& context, game_engine::GameEngine& eng) {
        int c;

        const char *title_format_str = "The terminal game template. Hp: %lu. Enemies: %lu. Score: %lu";

        std::vector<ScoreLine> records;
        get_score_board(records);

        while ('q' != (c = getch()) && !eng.need_to_stop()) {
            if (c == 'w') {
                save_game(eng, context);
                return;
            }
            if (c == 's') {
                while ('c' != (c = getch())) {
                    print_score_bord(records);
                }
            }
            eng.print_map(title_format_str);
            context.get_input(c);
            eng.reprint();
            context.change_count_enemies(eng.make_new_enemies());
            refresh();
        }

        ScoreLine cur;
        cur.score = context.get_score();
        clear();
        if (context.get_player_is_dead()) {
            std::string str = "game over";
            wmove(stdscr, 10, 15);
            wprintw(stdscr, "%s", str.c_str());
        } else if (context.get_count_enemies() == 0) {
            std::string str = "win";
            wmove(stdscr, 10, 15);
            wprintw(stdscr, "%s", str.c_str());
        }

        std::string str_sc = "Score: ";
        wmove(stdscr, 12, 15);
        wprintw(stdscr, "%s", str_sc.c_str());
        wprintw(stdscr, "%lu", cur.score);
        refresh();
        mvprintw(15, 15, "Write your nickname");
        int s;
        std::string name = "";
        while ('\n' != (s = getch())) {
            if (s == KEY_DC) {
                clear();
                wmove(stdscr, 15, 15);
                name.pop_back();
                wprintw(stdscr, "%s", name.c_str());
                refresh();
            } else if (isdigit(s) || isalpha(s)) {
                clear();
                wmove(stdscr, 15, 15);
                name += s;
                wprintw(stdscr, "%s", name.c_str());
                refresh();
            }
        }
        cur.name = name;
        save_score(cur, records);
        while ('c' != (c = getch())) {
            print_score_bord(records);
        }
    }

    void Game::print_menu() {
        clear();

        mvprintw(10, 10, " _____                    _____ _           _           ");
        mvprintw(11, 10, "|   __|___ ___ ___ ___   |   __| |_ ___ ___| |_ ___ ___ ");
        mvprintw(12, 10, "|__   | . | .'|  _| -_|  |__   |   | . | . |  _| -_|  _|");
        mvprintw(13, 10, "|_____|  _|__,|___|___|  |_____|_|_|___|___|_| |___|_|  ");
        mvprintw(14, 10, "      |_|");
        mvprintw(18, 10, "                 Press \"p\" to play");
        mvprintw(19, 10, "                 Press \"s\" to see score_board");
        mvprintw(20, 10, "                 Press \"w\" to continue saved game");
        mvprintw(21, 10, "                 Press enter to stop");
        refresh();
    }

    void Game::print_score_bord(std::vector<ScoreLine> &records) {
        clear();
        mvprintw(0, 1, "Press \"c\" to continue");
        int y = 2;
        mvprintw(y++, 10, "Top 10");
        for (auto it = records.begin(); it < records.end(); it++) {
            mvprintw(y++, 1, (it->name + " " + std::to_string(it->score)).c_str());
        }
        refresh();
    }

    void Game::get_score_board(std::vector<ScoreLine> &records) {
        std::ifstream score_board;
        score_board.open("ScoreBoard.txt");
        if (score_board.is_open()) {
            std::string line;
            while (std::getline(score_board, line)) {
                std::istringstream str_line(line);
                ScoreLine line;
                str_line >> line.name;
                str_line >> line.score;
                records.push_back(line);
            }
        }
    }

    void Game::save_score(ScoreLine &cur, std::vector<ScoreLine> &records) {
        bool is_inserted = false;
        for (auto it = records.begin(); it < records.end(); it++) {
            if (it != records.end()) {
                if (it->score < cur.score) {
                    records.insert(it, cur);
                    if (records.size() >= 10) {
                        records.pop_back();
                    }
                    is_inserted = true;
                    break;
                }
            }
        }
        if (!is_inserted && records.size() < 9) {
            records.push_back(cur);
        }

        std::ofstream score_board;
        score_board.open("ScoreBoard.txt");
        for (auto it = records.begin(); it < records.end(); it++) {
            score_board << it->name << " " << it->score << std::endl;
        }
    }

    void Game::save_game(game_engine::GameEngine& eng, game_context_objects::GameContext& cont) {
        std::vector<std::unique_ptr <game_context_objects::DynamicObject>>& obj = eng.get_obj();
        std::ofstream file;
        file.open("Saved_game.txt", std::ios::trunc | std::ios::out);
        file << 1 << std::endl;
        file << cont.get_score() << " " << cont.get_count_enemies() << " " << eng.get_add_count_enemies() << std::endl;
        std::string str;
        for (auto it = obj.begin(); it < obj.end(); it++) {
            it->get()->save_obj(str);
            file << str << std::endl;
        }
    }
}