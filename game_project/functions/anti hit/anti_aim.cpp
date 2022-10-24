#include "anti_aim.h"

#include "../config_vars.h"

#include "../aimbot/ragebot.h"
#include "../aimbot/autowall.h"

#include "../extra/function_checks.h"

#include "../listeners/listener_entity.h"

#include "../../base/tools/render.h"
#include "../../base/ingame objects/c_usercmd.h"
#include "../../base/ingame objects/c_animstate.h"
#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

namespace anti_aim
{
	bool change_pitch = false;

	int side = 0;
	bool flip = false;
	bool sidemove_flip = false;

	vector3d get_predicted_eye_pos()
	{
		vector3d velocity = g::local()->velocity();
		float speed = velocity.length(true);

		// predict eye position to choke * 2 ticks 
		// because we need to wait for unlag
		int predict_choke_amount = g_cfg.antihit.fakelag_limit * 2;

		vector3d start = g::local()->eye_pos() + 
			(speed > 5.f ? velocity * (math::ticks_to_time(predict_choke_amount)) : vector3d());

		return start;
	}

	c_csplayer* get_nearest_player()
	{
		auto player_array = listener_entity->get_entity(ent_player);
		if (player_array.empty())
			return nullptr;

		c_csplayer* best = nullptr;
		float best_dist = FLT_MAX;

		vector2d center = vector2d(render::screen.w * 0.5f, render::screen.h * 0.5f);

		for (auto player_info : player_array)
		{
			auto player = (c_csplayer*)player_info.entity;
			if (!player)
				continue;

			if (player->dormant() || !player->is_alive())
				continue;

			if (player == g::local() || player->team() == g::local()->team())
				continue;

			vector2d origin = {};
			if (!render::world_to_screen(player->origin(), origin))
				continue;

			float dist = center.distance_to_vector(origin);
			if (dist < best_dist)
			{
				best = player;
				best_dist = dist;
			}
		}

		return best;
	}
	
	std::array<int, 4> hitbox_list = 
	{
		hitbox_head,
		hitbox_chest,
		hitbox_stomach,
		hitbox_pelvis
	};

	bool can_peek()
	{
		auto player = get_nearest_player();
		if (!player)
			return false;

		if (g::local()->velocity().length(true) < 15.f)
			return false;

		for (auto h : hitbox_list)
		{
			auto pos = player->hitbox_position(h);
			auto info = autowall->wall_penetration(g::local()->eye_pos(), pos, player, true);
			if (info.damage > 0)
				return true;
		}
		return false;
	}

	void fake_duck()
	{
		if (!g_cfg.binds[fd_b].toggled)
			return;

		g::cmd->buttons |= in_bullrush;

		c_netchan* netchan = i::client_state->net_channel_ptr;

		int choked_packets = netchan->choked_packets;

		choked_packets > 6 ?
			g::cmd->buttons |= in_duck
			: g::cmd->buttons &= ~in_duck;
	}

	void fix_shoot_issues()
	{
		c_basecombatweapon* weapon = g::weapon();
		if (!weapon)
			return;

		short idx = weapon->definition_index();
		if (idx == weapon_revolver
			|| idx == weapon_c4
			|| idx == weapon_healthshot)
			return;

		weapon_info_t* weapon_info = weapon->get_weapon_info();
		if (!weapon_info)
			return;

		int type = weapon_info->weapon_type;
		if (type == 0 || type > 6)
			return;

		if (weapon->is_auto_sniper())
			return;

		if (misc_checks::is_able_to_shoot())
			return;

		if (g::cmd->buttons & in_attack)
			g::cmd->buttons &= ~in_attack;

		if (weapon->is_knife() && g::cmd->buttons & in_attack2)
			g::cmd->buttons &= ~in_attack2;
	}

	// when local stays at corner 
	// we can set sideways anti aim (and set pitch to zero sometimes)
	// to prevent hits to body
	bool can_hit_corner(float edge_pos, const vector3d& best_pos, const vector3d& eye_pos)
	{
		change_pitch = false;
	
		freestand_data_t traces = {};

		vector3d vector_angles[2] = {};
		math::angle_to_vectors(vector3d(0.f, edge_pos - 60.f, 0.f), vector_angles[0]);
		math::angle_to_vectors(vector3d(0.f, edge_pos + 60.f, 0.f), vector_angles[1]);

		auto init_trace = [](const vector3d& start, const vector3d& end, c_game_trace* out, bool debug)
		{
			c_trace_filter filter(g::local());
			i::engine_trace->trace_ray(ray_t(start, end), contents_solid | contents_grate, &filter, out);

			if (out->entity && (out->entity->is_player() || out->entity->is_weapon()))
				return;

			if (debug)
				i::debug_overlay->add_line_overlay(out->start, out->end, 255, 255, 255, true, 0.1f);
		};

		int predict_choke_amonunt = g_cfg.antihit.fakelag_limit;

		vector3d start = get_predicted_eye_pos();

		for (int i = 0; i < 2; i++)
			init_trace(start, start + vector_angles[i] * 300.f, &traces.trace[i], false);

		float left = traces.trace[0].fraction;
		float right = traces.trace[1].fraction;

		float diff = std::fabsf(left - right);

		// we didn't hit anything / have same corners
		if ((left == 1.f && right == 1.f) || diff <= 0.5f)
			return false;
		
		// head pos
		vector3d start_pos = eye_pos + vector3d(0.f, 0.f, 25.f);

		// how it looks:
		/*
		----------->
				  |
				  |
				  |
				  V
				object

		head
		*/

		// head trace above
		c_game_trace end_trace = {};
		init_trace(start_pos, best_pos + vector3d(0.f, 0.f, 25.f), &end_trace, false);

		// head trace below
		c_game_trace bottom_trace = {};
		init_trace(best_pos + vector3d(0.f, 0.f, 25.f), best_pos, &bottom_trace, false);

		// objects haven't found, let's set zero
		// TO-DO: do this only on peek
		if (end_trace.fraction >= 0.9f && bottom_trace.fraction >= 0.9f && can_peek())
			change_pitch = true;

		if (left > right)
			g::cmd->viewangles.y = edge_pos + 90.f;
		else if (right > left)
			g::cmd->viewangles.y = edge_pos - 90.f;

		flip = true;
		return true;
	}

	void do_wall_anti_aim()
	{
		bool is_on_ground = g::local()->flags() & fl_onground;

		// don't need to do this in air
		if (!g_cfg.antihit.edge || !is_on_ground)
		{
			change_pitch = false;
			return;
		}

		float best_dist = FLT_MAX;
		float best_yaw = FLT_MAX;
		vector3d best_pos = {};

		vector3d start = g::local()->get_abs_origin() + vector3d(0.f, 0.f, 50.f);

		for (float step = 0.f; step <= 2.f * M_PI; step += math::deg_to_rad(18.f))
		{
			float x = 40.f * std::cos(step);
			float y = 40.f * std::sin(step);

			vector3d end = vector3d(start.x + x, start.y + y, start.z);

			c_trace_filter filter(g::local());

			c_game_trace trace = {};
			i::engine_trace->trace_ray(ray_t(start, end), contents_solid | contents_grate, &filter, &trace);

			if (trace.entity && (trace.entity->is_player() || trace.entity->is_weapon()))
				continue;

			// find smallest fraction
			if (trace.fraction < 1.f)
			{
				float dist = start.distance_to_vector(trace.end);
				if (best_dist > dist && std::fabsf(best_dist - dist) > 0.5f)
				{
					best_yaw = math::rad_to_deg(step);
					best_pos = trace.end;
					best_dist = dist;
				}
			}
		}

		if (best_yaw == FLT_MAX)
		{
			change_pitch = false;
			return;
		}

		if (!can_hit_corner(best_yaw, best_pos, start))
		{
			change_pitch = false;
			g::cmd->viewangles.y = best_yaw;
		}
	}

	void trigger_move_for_desync()
	{
		if (g::local()->move_type() == movetype_noclip
			|| g::local()->move_type() == movetype_ladder)
			return;

		c_animstate* animstate = g::local()->get_animation_state();
		if (!animstate)
			return;

		if (!g_cfg.antihit.desync)
			return;

		float speed = g::local()->velocity().length(true);

		bool moving = g::cmd->buttons & in_moveleft
			|| g::cmd->buttons & in_moveright
			|| g::cmd->buttons & in_forward
			|| g::cmd->buttons & in_back;

		bool ready_to_move = !moving && speed < 5.f && (g::local()->flags() & fl_onground);
		if (ready_to_move)
		{
			// changing forwardmove values to make game think that we're moving
			// and use eye angles for abs yaw

			float modifier = (animstate->anim_duck_amount > 0.f || g_cfg.binds[fd_b].toggled) ? 3.f : 1.01f;
			g::cmd->forwardmove = sidemove_flip ? -modifier : modifier;
			sidemove_flip = !sidemove_flip;
		}
	}

	float total_yaw = 0.f;

	void fake_angle()
	{
		c_animstate* state = g::local()->get_animation_state();
		if (!state)
			return;

		if (!g_cfg.antihit.desync)
			return;

		if (g_cfg.antihit.invert)
			side = flip ? 1 : -1;
		else
			side = g_cfg.binds[inv_b].toggled ? 1 : -1;

		float max_body_rotation = 58.f;
		float current_body_rotation = 58.f;

		// last aviable desync angle
		float last_angle = math::normalize(g::cmd->viewangles.y + current_body_rotation * side);
		float eye_feet_delta = math::normalize(g::abs_rotation - last_angle);

		if (current_body_rotation < max_body_rotation && std::fabsf(eye_feet_delta) < max_body_rotation)
		{
			// add / substract angle so we make little jitter and abs yaw won't fuck up
			if (eye_feet_delta > 0.f)
				total_yaw = math::normalize(last_angle - max_body_rotation);
			else
				total_yaw = math::normalize(last_angle + max_body_rotation);
		}
		else
			total_yaw = math::normalize(last_angle + max_body_rotation * side);

		int max_tick = g_cfg.antihit.fakelag ? g_cfg.antihit.fakelag_limit : 1;

		if (!*g::send_packet && i::client_state->choked_commands < max_tick)
			g::cmd->viewangles.y = total_yaw;
	}


	void start()
	{
		fix_shoot_issues();
		trigger_move_for_desync();
	}

	void run()
	{
		if (g::local()->move_type() == movetype_noclip)
			return;

		fake_duck();

		if (misc_checks::is_firing())
			return;

		if (g::cmd->buttons & in_use)
			return;

		if (g_cfg.antihit.pitch)
			g::cmd->viewangles.x = change_pitch ? 0.f : 89.f;

		if (g_cfg.antihit.yaw)
			g::cmd->viewangles.y += 180.f;

		do_wall_anti_aim();
		fake_angle();

		if (*g::send_packet)
			flip = !flip;
	}

	void fix_angle_movement(c_usercmd* cmd, vector3d& ang)
	{
		if (!cmd)
			cmd = g::cmd;

		if (!ang.valid())
			ang = g::base_angle;

		vector3d move = vector3d(cmd->forwardmove, cmd->sidemove, 0.f);
		vector3d dir = {};

		float delta, len;
		vector3d move_angle = {};

		len = move.normalized_float();

		if (!len)
			return;

		math::vector_to_angles(move, move_angle);

		delta = (cmd->viewangles.y - ang.y);

		move_angle.y += delta;

		math::angle_to_vectors(move_angle, &dir);

		dir *= len;

		if (g::local()->move_type() == movetype_ladder)
		{
			if (cmd->viewangles.x >= 45 && ang.x < 45 && std::abs(delta) <= 65)
				dir.x = -dir.x;

			cmd->forwardmove = dir.x;
			cmd->sidemove = dir.y;

			if (cmd->forwardmove > 200)
				cmd->buttons |= in_forward;

			else if (cmd->forwardmove < -200)
				cmd->buttons |= in_back;

			if (cmd->sidemove > 200)
				cmd->buttons |= in_moveright;

			else if (cmd->sidemove < -200)
				cmd->buttons |= in_moveleft;
		}
		else
		{
			if (cmd->viewangles.x < -90 || cmd->viewangles.x > 90)
				dir.x = -dir.x;

			cmd->forwardmove = dir.x;
			cmd->sidemove = dir.y;
		}

		cmd->forwardmove = std::clamp(cmd->forwardmove, -450.f, 450.f);
		cmd->sidemove = std::clamp(cmd->sidemove, -450.f, 450.f);
		cmd->upmove = std::clamp(cmd->upmove, -320.f, 320.f);
	}

	void finish()
	{
		g::cmd->viewangles = math::normalize(g::cmd->viewangles);

		vector3d empty = {};
		fix_angle_movement(nullptr, empty);
	}
}