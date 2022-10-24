#include "exploits.h"
#include "fake_lag.h"

#include "../config_vars.h"
#include "../extra/function_checks.h"

#include "../../base/sdk.h"
#include "../../base/global_variables.h"

#include "../../base/ingame objects/c_usercmd.h"
#include "../../base/ingame objects/c_animstate.h"
#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

namespace fake_lag
{
	std::deque<command_t> commands = {};

	void run()
	{
		int choke = i::client_state->choked_commands;
		if (!g_cfg.antihit.fakelag || exploits::enabled())
		{
			*g::send_packet = g_cfg.antihit.desync ? choke >= 1 : true;
			return;
		}

		if (g_cfg.binds[fd_b].toggled || exploit[dt].recharging)
		{
			*g::send_packet = choke >= 13;
			return;
		}

		if (misc_checks::is_firing())
		{
			*g::send_packet = true;
			return;
		}

		*g::send_packet = false;

		// too much choked commands
		if (choke > 0
			&& std::abs(i::global_vars->tick_count - g::sent_tick_count) > g_cfg.antihit.fakelag_limit
			|| choke >= g_cfg.antihit.fakelag_limit - 1)
		{
			*g::send_packet = true;
			return;
		}
	}

	void fix_packets()
	{
		// get shit for packet start hook
		// and then send our choked packets for it
		auto& out = commands.emplace_back();
		out.is_outgoing = *g::send_packet;
		out.is_used = false;
		out.cmd_number = g::cmd->command_number;
		out.previous_command_number = 0;
		while (commands.size() > (int)(1.f / i::global_vars->interval_per_tick))
			commands.pop_front();

		if (!*g::send_packet)
		{
			auto net_channel = i::client_state->net_channel_ptr;

			if (net_channel->choked_packets > 0 && !(net_channel->choked_packets % 4))
			{
				auto backup_choke = net_channel->choked_packets;
				net_channel->choked_packets = 0;

				net_channel->send_datagram();
				--net_channel->out_sequence_nr;

				net_channel->choked_packets = backup_choke;
			}
		}
	}
}