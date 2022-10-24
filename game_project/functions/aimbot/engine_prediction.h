#pragma once
#include "../../base/sdk.h"

#include "../../base/tools/math.h"

#include "../../base/other/game_functions.h"

#include "../../base/ingame objects/c_usercmd.h"
#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

#include <memory>
#undef local

namespace net_compression
{
	struct stored_data_t
	{
		int tick_base = -1;
		float vel_modifier = -1.f;
		vector3d punch = {};
		vector3d punch_vel = {};
		vector3d view_offset = {};
	};

	void store();
	void set(int stage);
}

namespace engine_prediction
{
	void start();
	void end();
}