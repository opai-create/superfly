#pragma once
#include <memory>

#include "setup_bones_manager.h"

#include "../../base/ingame objects/c_csplayer.h"

#include "../../base/tools/math.h"
#include "../../base/tools/memory.h"

#include "../../base/other/game_functions.h"

namespace anim_correction
{
	extern c_setupbones bones;

	extern c_animationlayer layers[13];
	extern std::array<float, 24> poses;

	void force_update(c_csplayer* player, bool lag = true, bool animstate = false, const vector3d& angle = {});
	void update_render_data();
	void start(int stage);
}