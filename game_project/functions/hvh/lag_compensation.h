#pragma once
#include <memory>

#include "../../base/ingame objects/c_csplayer.h"

#include "../../base/tools/math.h"
#include "../../base/tools/memory.h"

#include "../../base/other/game_functions.h"

#include "setup_bones_manager.h"

#include <deque>

#include <unordered_map>

enum layers_t
{
	zero_side,
	left_side,
	right_side,
	original_side
};

struct record_t
{
	record_t() = default;
	explicit record_t(c_csplayer* player);

	void parse(c_csplayer* player);

	void restore(c_csplayer* player) const;
	void apply(c_csplayer* player) const;

	void setup_bones(c_csplayer* player, int idx);

	bool is_valid(float range, float max_unlag);

	c_csplayer* player{};
	int32_t index{};

	bool valid{};

	bool dormant{};
	bool low_delta{};
	bool on_ground{};

	vector3d velocity;

	vector3d obb_maxs;
	vector3d obb_mins;

	vector3d abs_origin;
	vector3d origin;

	vector3d eye_angles;
	vector3d abs_ang;
	vector3d origin_diff;

	c_animstate* anim_state;
	std::array<float, 24> poses;

	float old_simtime{};
	float sim_time{};
	float primary_cycle{};
	float move_weight{};
	float duck{};
	float lby{};
	float thirdperson_recoil{};
	float last_shot_time{};
	float time_delta{};
	float anim_speed{};
	float land_time{};

	int flags{};
	int eflags{};
	int effects{};

	float feet_cycle{};
	float feet_weight{};

	int lag{};

	bool was_in_air{};
	bool landing{};
	bool landed{};
	bool didshot{};

	c_setupbones bone_setup[4]{};

	c_animationlayer layers[4][13];

	alignas(16) matrix3x4_t bones[256];
	alignas(16) matrix3x4_t unresolved_bones[256];
	alignas(16) matrix3x4_t left_bones[256];
	alignas(16) matrix3x4_t right_bones[256];
};

class c_lag_compensation
{
public:
	struct animation_info
	{
		animation_info(c_csplayer* player, std::deque<record_t> animations)
			: player(player), frames(std::move(animations)), last_spawn_time(0), current(nullptr), last(nullptr), previous(nullptr)
		{ }

		record_t* current{};
		record_t* last{};
		record_t* previous{};

		void update_animations(int idx);
		void update();

		c_csplayer* player{};
		std::deque<record_t> frames{};
		float last_spawn_time{};
	};

	std::unordered_map<uint32_t, animation_info*> animation_infos;

	float get_lerp_time();

	void store_records(int stage);

	record_t* get_latest_record(c_csplayer* player);
	record_t* get_oldest_record(c_csplayer* player);
};

extern c_lag_compensation* lag_compensation;