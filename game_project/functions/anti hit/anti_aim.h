#pragma once
#include "../../base/sdk.h"
#include "../../base/global_variables.h"

class c_game_trace;
class ray_t;

struct freestand_data_t
{
	c_game_trace trace[2];
	ray_t ray[2];
};

namespace anti_aim
{
	void start();
	void run();
	void finish();
	void fix_angle_movement(c_usercmd* cmd, vector3d& ang);
}