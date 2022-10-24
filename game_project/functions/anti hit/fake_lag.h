#pragma once
#include <deque>

struct command_t
{
	int previous_command_number = 0;
	int cmd_number = 0;
	bool is_used = false;
	bool is_outgoing = false;
};

namespace fake_lag
{
	extern std::deque<command_t> commands;
	void run();
	void fix_packets();
}