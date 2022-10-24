#pragma once
#include <memory>

#include "../../base/ingame objects/c_csplayer.h"

#include "../../base/tools/math.h"
#include "../../base/tools/memory.h"

#include "../../base/other/game_functions.h"

#include <deque>

struct record_t;

namespace resolver
{
	struct resolver_info_t
	{
		bool got = false;
		bool got_layer = false;
		bool no_sidemove = false;

		int side = 0;
		int last_side = 0;

		float angle = 0.f;
		float old_feet_cycle = 0.f;
		float cycle_change_time = 0.f;
		float last_swtich_time = 0.f;

		c_animationlayer pre_orig[13] = {};

		void reset()
		{
			this->side = 0;
			this->last_side = 0;

			this->angle = 0.f;
			this->old_feet_cycle = 0.f;
			this->cycle_change_time = 0.f;
			this->last_swtich_time = 0.f;

			std::memset(this->pre_orig, 0, sizeof(pre_orig));

			this->got = false;
			this->got_layer = false;
			this->no_sidemove = false;
		}
	};

	extern std::array<resolver_info_t, 65> info;

	void on_round_start(c_game_event* event);
	void init(c_csplayer* player, record_t* record, resolver_info_t& info);
	void start(c_csplayer* player);
}
