#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"
#include "../../../functions/config_vars.h"

#include "../../../functions/aimbot/ragebot.h"
#include "../../../functions/aimbot/engine_prediction.h"

#include "../../../functions/anti hit/fake_lag.h"
#include "../../../functions/anti hit/anti_aim.h"
#include "../../../functions/anti hit/exploits.h"

#include "../../../functions/visuals/local/local_player_visuals.h"

#include "../../../functions/hvh/anim_correction.h"
#include "../../../functions/hvh/lag_compensation.h"

#include "../../../functions/extra/movement.h"
#include "../../../functions/extra/auto_revolver.h"
#include "../../../functions/extra/function_checks.h"

#include "../../../base/ingame objects/c_csplayer.h"
#include "../../../base/ingame objects/c_basecombatweapon.h"
#include "../../../base/ingame objects/c_usercmd.h"

#include "../../../functions/listeners/listener_entity.h"

#include <string>

#ifdef _DEBUG
void draw_capsule(c_csplayer* player, matrix3x4_t* bones, int idx, int idx2 = 1)
{
	studio_hdr_t* studio_model = i::model_info->get_studio_model(player->get_model());
	if (!studio_model)
		return;

	mstudio_hitbox_set_t* hitbox_set = studio_model->get_hitbox_set(0);
	if (!hitbox_set)
		return;

	for (int i = 0; i < hitbox_set->hitboxes; i++)
	{
		mstudio_bbox_t* hitbox = hitbox_set->get_hitbox(i);
		if (!hitbox)
			continue;

		vector3d vMin, vMax;
		math::vector_transform(hitbox->bbmin, bones[hitbox->bone], vMin);
		math::vector_transform(hitbox->bbmax, bones[hitbox->bone], vMax);

		if (hitbox->radius != -1.f)
			i::debug_overlay->add_capsule_overlay(vMin, vMax,
				hitbox->radius,
				255 * idx2,
				255 * idx,
				0,
				150, 5.f, 0, 1);
	}
}

void draw_lc_debug()
{
	auto player_array = listener_entity->get_entity(ent_player);
	if (player_array.empty())
		return;

	for (auto player_info : player_array)
	{
		auto player = (c_csplayer*)player_info.entity;
		if (!player)
			continue;

		if (player == g::local() || player->team() == g::local()->team())
			continue;

		if (!player->is_alive())
			continue;

		int index = player->index();

		auto last = lag_compensation->get_latest_record(player);
		if (!last)
			continue;

		if (misc_checks::is_firing())
		{
			draw_capsule(player, last->left_bones, 0, 0);
			draw_capsule(player, last->right_bones, 0);
			draw_capsule(player, last->unresolved_bones, 1);
		}
	}
}
#endif

namespace tr
{
	namespace client_mode
	{
		void __fastcall override_view(void* ecx, void* edx, c_view_setup* setup)
		{
			static auto original = vtables[vtables_t::client_mode].original<override_view_fn>(18);

			visuals::local::change_view(setup);
			original(ecx, setup);
			visuals::local::view_after_orig();
		}

		float __fastcall get_viewmodel_fov(void* ecx, void* edx)
		{
			static auto original = vtables[vtables_t::client_mode].original<get_viewmodel_fov_fn>(35);
			return original(ecx) + g_cfg.misc.fovs[arms];
		}

		int __fastcall set_view_model_offsets(void* ecx, void* edx, int something, float x, float y, float z)
		{
			static auto original = hooker.original(&set_view_model_offsets);

			if (g::local() && g::local()->is_alive())
			{
				x += g_cfg.misc.viewmodel_pos[0];
				y += g_cfg.misc.viewmodel_pos[1];
				z += g_cfg.misc.viewmodel_pos[2];
			}

			return original(ecx, edx, something, x, y, z);
		}

		bool __fastcall draw_fog(void* ecx, void* edx)
		{
			static auto original = hooker.original(&draw_fog);

			if (g_cfg.misc.removals & fog_)
				return false;

			return original(ecx, edx);
		}

		int last_choked_packets = -1;
	
		bool __fastcall create_move(void* ecx, void* edx, float input_sample_frametime, c_usercmd* cmd)
		{
			static auto original = vtables[vtables_t::client_mode].original<create_move_fn>(24);

			g::modify_eye_pos = false;

			if (!cmd || !cmd->command_number)
				return original(ecx, input_sample_frametime, cmd);

			if (!g::local() || !g::local()->is_alive() || !g::weapon())
				return original(ecx, input_sample_frametime, cmd);

			if (original(ecx, input_sample_frametime, cmd))
			{
				i::engine->set_view_angles(cmd->viewangles);
				i::prediction->set_local_view_angles(cmd->viewangles);
			}

			g::modify_eye_pos = true;

			g::cmd = cmd;
			g::base_angle = cmd->viewangles;

			g::tick_rate = 1.f / i::global_vars->interval_per_tick;

			if (exploits::need_to_recharge && last_choked_packets == 0)
			{
				cmd->tickcount = INT_MAX;

				cmd->forwardmove = 0.0f;
				cmd->sidemove = 0.0f;
				cmd->upmove = 0.0f;

  				cmd->buttons &= ~(in_attack | in_attack2);

				if (++exploits::charge_ticks >= 16)
				{
					exploits::need_to_recharge = false;
					exploits::recharge_completed = true;
					tick_base.store(g::local()->tick_base(), g::cmd->command_number, exploits::charge_ticks, false);
					*g::send_packet = true;
				}
				else
					*g::send_packet = false;

				exploits::shift_amount = 0;
				last_choked_packets = 0;
				return false;
			}
			else
				last_choked_packets = i::client_state->choked_commands;

			i::prediction->update(i::client_state->delta_tick,
				i::client_state->delta_tick > 0,
				i::client_state->last_command_ack,
				i::client_state->last_outgoing_command + i::client_state->choked_commands);

			exploits::prepare();

			if (exploit[hs].works && !g::weapon()->is_misc_weapon())
				g::tick_base = g::local()->tick_base() - 9;
			else
				g::tick_base = g::local()->tick_base();

			movement::run();
			fake_lag::run();

			auto_revolver::calc_time();
			anti_aim::start();
#ifdef _DEBUG
			draw_lc_debug();
#endif

			engine_prediction::start();
			{
				ragebot::run();
				anti_aim::run();
			}
			exploits::run();

			engine_prediction::end();

			g::modify_eye_pos = false;

			anti_aim::finish();
			g::cmd_angle = g::cmd->viewangles;

			if (*g::send_packet)
				g::sent_tick_count = i::global_vars->tick_count;

			g::update_local = true;

			if (i::client_state->choked_commands > 14)
				*g::send_packet = true;

			anim_correction::start(frame_render_start);

			fake_lag::fix_packets();

			cmd = g::cmd;
			return false;
		}
	}
}