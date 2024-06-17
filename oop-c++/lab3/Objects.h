#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <curses.h>

#include <chrono>
#include <vector>
#include <algorithm>

const int p_h = 1;
const int p_w = 3;
const int e_h = 1;
const int e_w = 3;
const std::string s_enemy = "\\E/";
const std::string s_player = "/H\\";

using steady_clock_t = std::chrono::time_point<std::chrono::steady_clock>;

namespace game_context_objects {
    class DynamicObject;
    std::unique_ptr<DynamicObject> create_object(const std::string& line, int h, int w, int hp);

    class GameContext {
    private:
        std::vector<std::unique_ptr <DynamicObject>> obj;
        int hmax;
        int wmax;
        std::string dest;
        int cur_index = 0;
        int score = 0;
        int count_enemies = 0;
        bool pressed = false;
        bool player_is_dead = false;
        //bool stop = false;
    public:
        GameContext(int h1, int w1) : dest{""}, hmax{h1}, wmax{w1} {
            obj.push_back(create_object("player", (wmax - p_w)/ 2, hmax - 1 - p_h, 3));
        };
        GameContext(int h1, int w1, int sc, int c_en) : dest{""}, hmax{h1}, wmax{w1}, score{sc}, count_enemies{c_en} {};

        std::vector<std::unique_ptr <DynamicObject>>& get_game_objects();
        void get_input(int c);
        int get_cur_index() { return cur_index; };
        int get_count_enemies() { return count_enemies; };
        int get_hmax() { return hmax; };
        int get_wmax() { return wmax; };
        int get_score() { return score; };
        bool get_player_is_dead() { return player_is_dead; };
        void player_dead() { player_is_dead =  true; }
        //bool need_to_stop() { if (player_is_dead) {return true; } return false; };
        bool get_pressed();
        std::string& get_dest();
        void change_count_enemies(int param);
        void change_cur_index(int param) { cur_index = param; };
        void increase_score() { score++; };
    };

	class Object {
	protected:
		int t_x;
		int t_y;
        int t_w = 1;
        int t_h = 1;
        int t_hp = 1;
        std::vector<std::string> t_str;
	public:
		Object() : t_x{ 0 }, t_y{ 0 } { t_str.push_back("*"); };
		Object(std::string str) : t_x{ 0 }, t_y{ 0 } { t_str.push_back(str); };
		Object(int x, int y, std::string str) : t_x{ x }, t_y{ y } { t_str.push_back(str); };
        Object(int x, int y, std::string str, int w, int h, int hp) : t_x{ x }, t_y{ y }, t_w{ w }, t_h{ h }, t_hp{ hp } { t_str.push_back(str); };

		virtual ~Object() = default;
	};

	class DynamicObject : public Object{
	public:
        DynamicObject() : Object() {};
        DynamicObject(std::string str) : Object(str) {};
        DynamicObject(int x, int y, std::string str) : Object(x, y, str) {};
        DynamicObject(int x, int y, std::string str, int w, int h, int hp) : Object(x, y, str, w, h, hp) {};

        virtual void one_step_change(GameContext& cont) {};
        int get_hp() {return t_hp; };
		int get_x() { return t_x; };
		int get_y() { return t_y; };
        int get_h() { return t_h; };
        int get_w() { return t_w; };
        virtual bool is_enemy() { return false; };
		virtual void move(const std::string& dest);
		virtual void print(int h1, int w1);
		virtual void hurt() { if (t_hp != 0) { t_hp--; } };
		virtual bool is_alive() { return t_hp != 0; };
		virtual bool end_map(int h1) {
			return (t_y < 2 || t_y > h1 - 2 - t_h);
		}
        virtual void catch_collisions(GameContext& cont);
        virtual void save_obj(std::string& str) {};
	};

	class Bullet : public DynamicObject {
	private:
		steady_clock_t last_time;
		bool change;
	public:
        void one_step_change(GameContext& cont) override;
		Bullet() : DynamicObject("*"), last_time(std::chrono::steady_clock::now()) {};
		Bullet(int x, int y, int hp) : DynamicObject(x, y, "*", 1, 1, hp), last_time(std::chrono::steady_clock::now()) {};
		bool need_to_change();
		void change_time();
		bool end_map(int h1) override {
			return (t_y < 5 || t_y > h1 - 2 - t_h);
		}
        void save_obj(std::string& str) override {
            str = "bullet " + std::to_string(t_x) + " " + std::to_string(t_y) + " " + std::to_string(t_hp);
        }
	};

    class EnemyBullet : public DynamicObject {
    private:
        steady_clock_t last_time;
        bool change;
    public:
        void one_step_change(GameContext& cont) override;
        EnemyBullet(int x, int y, int hp) : DynamicObject(x, y, "|", 1, 1, hp), last_time(std::chrono::steady_clock::now()) {};
        bool need_to_change();
        void change_time();
        bool end_map(int h1) override {
            return (t_y > h1 - 2 - t_h);
        }
        void catch_collisions(GameContext& cont) override {};
        void save_obj(std::string& str) override {
            str = "enemy_bullet " + std::to_string(t_x) + " " + std::to_string(t_y) + " " + std::to_string(t_hp);
        }
    };

	class Player : public DynamicObject {
	public:
		Player(int x, int y, int hp) : DynamicObject(x, y, s_player, p_w, p_h, hp) {};

        void one_step_change(GameContext& cont) override;
        void save_obj(std::string& str) override {
            str = "player " + std::to_string(t_x) + " " + std::to_string(t_y) + " " + std::to_string(t_hp);
        }
	};

	class Enemy : public DynamicObject {
	private:
		steady_clock_t last_time;
        steady_clock_t shoot_last_time;
		bool change;
	public:
		Enemy() : DynamicObject(s_enemy), last_time(std::chrono::steady_clock::now()), shoot_last_time(std::chrono::steady_clock::now()) {};
		Enemy(int x, int y, int hp) : DynamicObject(x, y, s_enemy, e_w, e_h, hp), last_time(std::chrono::steady_clock::now()), shoot_last_time(std::chrono::steady_clock::now()) {};
        void one_step_change(GameContext& cont) override;
		bool need_to_change();
		void change_time();
		bool end_map(int h1) override {
			return (t_y > h1 - 2 - t_h);
		}
        bool is_enemy() override { return true; };
        void catch_collisions(GameContext& cont) override {};
        void save_obj(std::string& str) override {
            str = "enemy " + std::to_string(t_x) + " " + std::to_string(t_y) + " " + std::to_string(t_hp);
        }
	};
}