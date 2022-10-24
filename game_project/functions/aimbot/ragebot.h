#pragma once
#include <array>

#include "../hvh/lag_compensation.h"
#include "../../base/tools/math.h"

class c_csplayer;

struct best_target_t
{
	bool filled = false;
	vector3d point = {};
	int hitbox = -1;
	record_t* record;
	int damage = -1;

	void get(const vector3d& point, int hitbox, record_t* record, int damage)
	{
		this->point = point;
		this->hitbox = hitbox;
		this->record = record;
		this->damage = damage;

		this->filled = true;
	}

	void reset()
	{
		this->point.reset();
		this->hitbox = -1;
		this->damage = -1;
		this->record = nullptr;

		this->filled = false;
	}
};

enum scan_type_t
{
	last_tick = 0,
	old_tick,
};

struct tick_selection_t
{
	vector3d point = {};
	int damage = 0;
	int hitbox = 0;

	tick_selection_t() = default;

	tick_selection_t(const vector3d& point, int damage, int hitbox)
	{
		this->point = point;
		this->damage = damage;
		this->hitbox = hitbox;
	}
};

struct aimbot_data_t 
{
	// get data from center point and multipoints
	vector3d points[2] = {};
	int damages[2] = {};
	int hitbox = -1;
	bool body = false;
	record_t* record = nullptr;

	aimbot_data_t() = default;

	aimbot_data_t(const vector3d& center, const vector3d& multi,
		int center_dmg, int dmg, int hitbox, bool body, record_t* record)
	{
		this->points[0] = center;
		this->points[1] = multi;

		this->damages[0] = center_dmg;
		this->damages[1] = dmg;

		this->hitbox = hitbox;

		this->record = record;
	}
};

namespace aim_utils
{
	void prepare_seed();
	void on_round_events(c_game_event* event);

	int hitbox_to_hitgroup(int hitbox);
}

namespace ragebot
{
	void store(c_csplayer* player);
	void set(c_csplayer* player, record_t* record);
	void restore(c_csplayer* player);

	void auto_stop();
	void run();
}