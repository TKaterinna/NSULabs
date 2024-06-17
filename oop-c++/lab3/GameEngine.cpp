#include "GameEngine.h"

namespace game_engine {

    void out(int y, int x, const std::string& s) {
        wmove(stdscr, y, x);
        wprintw(stdscr, "%s", s.c_str());
    }

    int GameEngine::make_new_enemies() {
        if ( add_count_enemies > 0) {
            if (rand() % 100 == 0) {
                //int count = rand() % 4;
                int count = 1;
                std::vector<std::unique_ptr<game_context_objects::DynamicObject>> &obj = get_obj();
//                if (add_count_enemies < count) {
//                    count = add_count_enemies;
//                }
                for (int i = 0; i < count; i++) {
                    obj.push_back(game_context_objects::create_object("enemy", rand() % (cont.get_wmax()- e_w), 4, 1));
                }
                add_count_enemies -= count;
                return count;
            }
        }
        return 0;
    }

    void GameEngine::reprint() {
        std::vector<std::unique_ptr <game_context_objects::DynamicObject>>& obj = get_obj();
        for (int i = 0; i < obj.size(); i++) {
            cont.change_cur_index(i);
            obj[i]->one_step_change(cont);
        }
        for (int i = 0; i < obj.size(); ) {
            cont.change_cur_index(i);
            obj[i]->catch_collisions(cont);
            if (!obj[i]->is_alive()) {
                obj.erase(obj.begin() + i);
            } else {
                i++;
            }
        }
    }

    void GameEngine::print_map(const char* title_format_str) {
        clear();
        int wmax = cont.get_wmax();
        int hmax = cont.get_hmax();
        std::vector<std::unique_ptr <game_context_objects::DynamicObject>>& obj = get_obj();
        int title_size = snprintf(nullptr, 0, title_format_str, obj[0]->get_hp(),
                                  cont.get_count_enemies(), cont.get_score());
        wmove(stdscr, 2, (wmax - title_size) / 2);
        wprintw(stdscr, title_format_str, obj[0]->get_hp(), cont.get_count_enemies(), cont.get_score());

        wmove(stdscr, 1, 0);
        mvprintw(1, 0, "\"s\" - score board; \"q\" - exit; \"w\" - save");

        for (int i = 1; i < wmax; ++i) {
            out(3, i, "-");
            out(hmax - 1, i, "-");
        }
        for (int i = 3; i < hmax - 1; ++i) {
            out(i, 0, "|");
            out(i, wmax - 1, "|");
        }
        out(3, 0, "+");
        out(3, wmax - 1, "+");
        out(hmax - 1, 0, "+");
        out(hmax - 1, wmax - 1, "+");

        refresh();
    }
}