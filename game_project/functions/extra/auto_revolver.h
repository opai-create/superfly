#pragma once
#include <array>

class c_csplayer;
class c_usercmd;

struct revolver_data_t
{
	std::array<int, 150> tickbase;
	std::array<bool, 150> in_attacks;
	std::array<bool, 150> can_shoot;

	void get(c_csplayer* player, c_usercmd* cmd);
};

extern revolver_data_t revolver_data;

namespace auto_revolver
{
	void update_revolver_time(int cmd_number);
	void calc_time();
}