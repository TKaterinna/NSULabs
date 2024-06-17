#include "Objects.h"
#include <iostream>

#include <stdio.h>
#include <curses.h>

#include <chrono>
#include <vector>
#include <algorithm>

using namespace std::chrono_literals;
using steady_clock_t = std::chrono::time_point<std::chrono::steady_clock>;

steady_clock_t now() {
	return std::chrono::steady_clock::now();
}

namespace game_context_objects {
	void DynamicObject::move(const std::string& dest) {
		if (dest == "up") {
			t_y--;
		}
		else if(dest == "down") {
			t_y++;
		}
		else if (dest == "right") {
			t_x++;
		}
		else if (dest == "left") {
			t_x--;
		}
	}

	void DynamicObject::print(int h1, int w1) {
		t_y = std::clamp(t_y, 1, h1 - 1 - t_h);
		t_x = std::clamp(t_x, 1, w1 - 1 - t_w);
        int y = t_y;
        for (auto i = t_str.begin(); i != t_str.end(); i++) {
            wmove(stdscr, t_y, t_x);
            wprintw(stdscr, "%s", i->data());
            y++;
        }
	}

	bool Bullet::need_to_change() {
		return (now() - last_time) / 1ms > 200;
	}
	void Bullet::change_time() {
		last_time = now();
	}
    bool EnemyBullet::need_to_change() {
        return (now() - last_time) / 1ms > 201;
    }
    void EnemyBullet::change_time() {
        last_time = now();
    }

	bool Enemy::need_to_change() {
		return (now() - last_time) / 1ms > 1000;
	}
	void Enemy::change_time() {
		last_time = now();
	}

    void Bullet::one_step_change(GameContext& cont) {
        if (need_to_change()) {
            move("up");
            change_time();
        }
        print(cont.get_hmax(), cont.get_wmax());
        if (end_map(cont.get_hmax())) {
            t_hp = 0;
        }
    }
    void EnemyBullet::one_step_change(GameContext& cont) {
        if (need_to_change()) {
            move("down");
            change_time();
        }
        print(cont.get_hmax(), cont.get_wmax());
        if (end_map(cont.get_hmax())) {
            t_hp = 0;
        }
    }
    void Player::one_step_change(GameContext& cont) {
        if (cont.get_pressed()) {
            if (cont.get_dest() != "bullet") {
                move(cont.get_dest());
            }
            else {
                cont.get_game_objects().push_back(create_object("bullet", t_x + (t_w / 2), t_y - 1, 1));
            }
            std::vector<std::unique_ptr<DynamicObject>>& obj = cont.get_game_objects();
        }
        if (t_hp == 0) {
            cont.player_dead();
        }
        print(cont.get_hmax(), cont.get_wmax());
    }
    void Enemy::one_step_change(GameContext& cont) {
        if (need_to_change()) {
            move("down");
            change_time();
        }
        else if (((now() - shoot_last_time) / 1ms > 2100) && (rand() % 60 == 0)) {
            cont.get_game_objects().push_back(create_object("enemy_bullet", t_x + (t_w / 2),  t_y + 2, 1));
            shoot_last_time = now();
        }
        print(cont.get_hmax(), cont.get_wmax());
        if (end_map(cont.get_hmax())) {
            cont.get_game_objects()[0]->hurt();
            t_hp = 0;
        }
        if (t_hp == 0) {
            cont.change_count_enemies(-1);
        }
    }

    void DynamicObject::catch_collisions(GameContext& cont) {
        std::vector<std::unique_ptr<DynamicObject>>& obj = cont.get_game_objects();
        int cur_index = cont.get_cur_index();
        for (int i = 0; i < obj.size(); i++) {
            if (i != cur_index) {
                for (int j1 = 0; j1 < obj[i]->get_h(); j1++) {
                    for (int j2 = 0; j2 < obj[cur_index]->get_h(); j2++) {
                        if (obj[i]->get_y() + j1 == obj[cur_index]->get_y() + j2) {
                            for (int k1 = 0; k1 < obj[i]->get_w(); k1++) {
                                for (int k2 = 0; k2 < obj[cur_index]->get_w(); k2++) {
                                    if (obj[i]->get_x() + k1 == obj[cur_index]->get_x() + k2) {
                                        if (obj[i]->is_enemy()) {
                                            cont.increase_score();
                                        }
                                        obj[i]->hurt();
                                        obj[cur_index]->hurt();
                                        if (t_hp == 0 && cur_index == 0) {
                                            cont.player_dead();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    std::unique_ptr<DynamicObject> create_object(const std::string& line, int x, int y, int hp) {
        if (line == "player") {
            return std::make_unique<Player>(x, y, hp);
        }
        else if (line == "bullet") {
            return std::make_unique<Bullet>(x, y, hp);
        }
        else if (line == "enemy_bullet") {
            return std::make_unique<EnemyBullet>(x, y, hp);
        }
        else if (line == "enemy") {
            return std::make_unique<Enemy>(x, y, hp);
        }
        return nullptr;
    }
}