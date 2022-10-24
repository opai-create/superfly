#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"

#include "../../../functions/config_vars.h"
#include "../../../functions/extra/auto_revolver.h"
#include "../../../functions/aimbot/engine_prediction.h"

#include "../../../base/ingame objects/c_csplayer.h"

namespace tr
{
	namespace prediction
	{
		bool __fastcall in_prediction(void* ecx, void* edx)
		{
			static auto original = vtables[vtables_t::prediction].original<in_prediction_fn>(14);

			static auto weapon_recoil_scale = i::convar->find_convar("weapon_recoil_scale");
			static auto view_recoil_tracking = i::convar->find_convar("view_recoil_tracking");

			if (!g::in_game || !(g_cfg.misc.removals & vis_recoil))
				return original(ecx);

			if (!g::local() || !g::local()->is_alive())
				return original(ecx);

			// idk what it does in game
			// anyway we should turn it off
			if ((uintptr_t)_ReturnAddress() == patterns[pattern_t::return_addr_drift_pitch].as<uintptr_t>())
				return true;

			if ((uintptr_t)_ReturnAddress() == patterns[pattern_t::return_addr_apply_shake].as<uintptr_t>())
			{
				volatile float* angles = nullptr;
				_asm mov angles, ebx;

				const vector3d recoil_angle = g::local()->view_punch_angle()
					+ (g::local()->aim_punch_angle() * weapon_recoil_scale->get_float())
					* view_recoil_tracking->get_float();

				if (angles)
				{
					angles[0] -= recoil_angle.x;
					angles[1] -= recoil_angle.y;
					angles[2] -= recoil_angle.z;
				}

				return true;
			}

			return original(ecx);
		}

		void __fastcall run_command(void* ecx, void* edx, c_csplayer* player, c_usercmd* cmd, c_movehelper* move_helper)
		{
			static auto original = vtables[vtables_t::prediction].original<run_command_fn>(19);
			if (!g::in_game || !g::local() || !g::local()->is_alive() || !g::weapon() || player != g::local())
				return original(ecx, player, cmd, move_helper);

			if (cmd->tickcount >= i::global_vars->tick_count + g::tick_rate + 8)
			{
				// don't predict netvars on recharge
				cmd->predicted = true;

				// still process local tickbase
				++player->tick_base();
				return;
			}

			g::modify_eye_pos = true;

			// get unpredicted data for autorevolver
			revolver_data.get(player, cmd);

			// update post pone fire time
			auto_revolver::update_revolver_time(cmd->command_number);
			original(ecx, player, cmd, move_helper);

			// store predicted netvars
			net_compression::store();

			g::modify_eye_pos = false;
		}
	}
}