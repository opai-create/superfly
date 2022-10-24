#include "auto_revolver.h"
#include "function_checks.h"

#include "../config_vars.h"

#include "../../base/ingame objects/c_usercmd.h"
#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

#include "../../base/tools/math.h"
#include "../../base/tools/memory.h"

#include "../../base/other/game_functions.h"

revolver_data_t revolver_data = {};

void revolver_data_t::get(c_csplayer* player, c_usercmd* cmd)
{
	int cmd_number = cmd->command_number % 150;
	tickbase[cmd_number] = player->tick_base();
	in_attacks[cmd_number] = cmd->buttons & 0x801;
	can_shoot[cmd_number] = misc_checks::is_able_to_shoot();
}

namespace auto_revolver
{
    float last_cock_time = 0.f;

	void update_revolver_time(int cmd_number)
	{
        if (!(g_cfg.rage.auto_ & 8))
            return;

		if (g::weapon()->definition_index() != weapon_revolver)
			return;

        float post_pone_time = FLT_MAX;

        int tickrate = (1.0 / i::global_vars->interval_per_tick);
        if (tickrate >> 1 > 1)
        {
            int cmd_num = cmd_number - 1;

            int new_cmd = 0;
            for (int i = 1; i < tickrate >> 1; ++i)
            {
                new_cmd = cmd_num;
                if (!revolver_data.in_attacks[cmd_num % 150] || !revolver_data.can_shoot[cmd_num % 150])
                    break;
                --cmd_num;
            }

            if (new_cmd)
            {
                float revolver_cock_time = 1.f - (-0.03348f / i::global_vars->interval_per_tick);
                if (cmd_number - new_cmd >= revolver_cock_time)
                {
                    int new_tick = (new_cmd + static_cast<int>(revolver_cock_time)) % 150;
                    post_pone_time = math::ticks_to_time(revolver_data.tickbase[new_tick]) + 0.2f;
                }
            }
        }

        if (std::isfinite(post_pone_time))
            g::weapon()->post_pone_fire_ready_time() = post_pone_time;
	}

	void calc_time()
    {
        if (!(g_cfg.rage.auto_ & 8))
            return;

        if (!g::weapon())
            return;

        if (g::weapon()->definition_index() != weapon_revolver)
            return;

        // how many ticks revolver cocks (converted to time)
        const float REVOLVER_COCK_TIME = 0.234375f;

        if (!(g::cmd->buttons & in_attack) && g::weapon()->current_ammo())
        {
            float curtime = math::ticks_to_time(g::local()->tick_base());

            g::cmd->buttons &= ~in_attack2;
            if (misc_checks::is_able_to_shoot())
            {
                if (curtime < last_cock_time)
                    g::cmd->buttons |= in_attack;
                else if (curtime < g::weapon()->next_secondary_attack())
                    g::cmd->buttons |= in_attack2;
                else
                    last_cock_time = curtime + REVOLVER_COCK_TIME;
            }
            else
            {
                last_cock_time = curtime + REVOLVER_COCK_TIME;
                g::cmd->buttons &= ~in_attack;
            }
        }
	}
}