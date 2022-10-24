#include "resolver.h"

#include "../aimbot/autowall.h"
#include "../aimbot/ragebot.h"

#include "../anti hit/anti_aim.h"

#include "../hvh/lag_compensation.h"

namespace resolver
{
	void on_round_start(c_game_event* event)
	{
		if (std::strcmp(event->get_name(), "round_start"))
			return;

		for (auto& i : info)
			i.reset();
	}

	std::array<resolver_info_t, 65> info = {};

	bool can_hit_corner(c_csplayer* player, resolver_info_t& info, float edge_pos, const vector3d& best_pos, const vector3d& eye_pos)
	{
		freestand_data_t traces = {};

		vector3d vector_angles[2] = {};
		math::angle_to_vectors(vector3d(0.f, edge_pos - 60.f, 0.f), vector_angles[0]);
		math::angle_to_vectors(vector3d(0.f, edge_pos + 60.f, 0.f), vector_angles[1]);

		auto init_trace = [&](const vector3d& start, const vector3d& end, c_game_trace* out, bool debug)
		{
			c_trace_filter filter(player);
			i::engine_trace->trace_ray(ray_t(start, end), contents_solid | contents_grate, &filter, out);

			if (out->entity && (out->entity->is_player() || out->entity->is_weapon()))
				return;

			if (debug)
				i::debug_overlay->add_line_overlay(out->start, out->end, 255, 255, 255, true, 0.1f);
		};

		for (int i = 0; i < 2; i++)
			init_trace(eye_pos, eye_pos + vector_angles[i] * 300.f, &traces.trace[i], false);

		float left = traces.trace[0].fraction;
		float right = traces.trace[1].fraction;

		float diff = std::fabsf(left - right);

		// we didn't hit anything
		if ((left == 1.f && right == 1.f))
			return false;

		if (left > right)
			info.last_side = -1;
		else if (right > left)
			info.last_side = 1;

		info.side = info.last_side;
		return true;
	}

	bool can_use_edge(c_csplayer* player, resolver_info_t& info)
	{
		float best_dist = FLT_MAX;
		float best_yaw = FLT_MAX;
		vector3d best_pos = {};

		vector3d start = player->get_abs_origin() + vector3d(0.f, 0.f, 50.f);

		for (float step = 0.f; step <= 2.f * M_PI; step += math::deg_to_rad(18.f))
		{
			float x = 40.f * std::cos(step);
			float y = 40.f * std::sin(step);

			vector3d end = vector3d(start.x + x, start.y + y, start.z);

			c_trace_filter filter(player);

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
			return false;

		if (!can_hit_corner(player, info, best_yaw, best_pos, start))
			return false;

		//log_console("edge \n");
		return true;
	}

	// i setup 2 points at left and right side
	// this method breaks on sideways (when enemy's head is rotated to 90 / -90 degrees)
	// but when enemy's head is in forward direction points won't flip 
	std::vector<vector3d> get_points(c_csplayer* player, record_t* record)
	{
		std::vector<vector3d> points;

		// bone index of head
		const int bone = 8;

		vector3d fw{};
		vector3d src = record->unresolved_bones[bone].get_origin();

		float at_target_yaw = math::angle_from_vectors(g::local()->origin(), player->origin()).y;
		math::angle_to_vectors(vector3d(0, at_target_yaw - 90.f, 0), fw);

		vector3d left_side = src - (fw * 40);
		vector3d right_side = src + (fw * 40);

		points.push_back(left_side);
		points.push_back(right_side);

		return points;
	}

	bool can_use_damage(c_csplayer* player, record_t* record, resolver_info_t& info)
	{
		auto p = get_points(player, record);

		ragebot::store(player);
		ragebot::set(player, record);
		auto info_a = autowall->wall_penetration(g::local()->eye_pos(), p[0], player, true);
		auto info_b = autowall->wall_penetration(g::local()->eye_pos(), p[1], player, true);
		ragebot::restore(player);

		if (std::fabs(info_a.damage - info_b.damage) < 10.f)
			return false;

		//log_console("dmg \n");
		info.last_side = info_a.damage > info_b.damage ? -1 : 1;
		info.side = info.last_side;
		return true;
	}

	void anti_freestanding(c_csplayer* player, record_t* record, resolver_info_t& info)
	{
		info.angle = player->get_animation_state()->get_max_body_rotation();

		if (!can_use_edge(player, info))
		{
			if (!can_use_damage(player, record, info))
				return;
		}
	}

	void animation_resolver(c_csplayer* player, record_t* record, resolver_info_t& info)
	{
		auto state = player->get_animation_state();
		if (!state)
			return;

		float orig_rate = info.pre_orig[6].playback_rate;
		float speed = player->velocity().length(true);

		// zero angle layer rate always have same value
		// but when enemy playing with desync - his rate is higher than simulated one
		// we should get difference between them and get precentage of desync amount

		// IMPORTANT: simulated layers give results that twice higher than original layer value

		float zero_rate = record->layers[zero_side][6].playback_rate;
		float left_rate = record->layers[left_side][6].playback_rate;
		float right_rate = record->layers[right_side][6].playback_rate;
		float delta = info.pre_orig[6].playback_rate - zero_rate;

		// round layer data to get correct values
		float round_zero = std::roundf(zero_rate * 1000000.f) / 1000000.f;
		float round_orig = std::roundf((zero_rate + delta) * 1000000.f) / 1000000.f;

		// desync value goes from 100 (no desync) to 94 (full delta desync)
		// we need to get value from 0 to 6
		float dsy_percent = (round_zero * 100.f / round_orig) - 94.f;

		// we got inversed desync range 
		// to get real desync range we should substract 60 from our value
		info.angle = std::clamp(std::fabsf(60.f - (dsy_percent * 10.f)), 0.f, 60.f);

		// low desync range, ignore enemy
		/*if (info.angle <= 20.f)
		{
			info.angle = 0.f;
			info.side = 0;
			info.got = true;
			return;
		}*/

		float left_delta = std::fabsf(left_rate - orig_rate);
		float right_delta = std::fabsf(right_rate - orig_rate);
		float both_delta = std::fabsf(left_delta - right_delta);

		float dsy = state->get_max_body_rotation();
	
		if (left_delta > right_delta)
			info.last_side = -1;
		else if (left_delta < right_delta)
			info.last_side = 1;

		log_console("left_delta: %0.6f | right_delta: %0.6f | both_delta: %0.6f | side: %d \n",
			left_delta, right_delta, both_delta, info.last_side);

		//log_console("side: %d \n", info.last_side);
		info.side = info.last_side;
	}

	void start(c_csplayer* player)
	{
		auto state = player->get_animation_state();
		if (!state)
			return;

		int idx = player->index() - 1;

		const auto& in = info[idx];
		if (!in.got)
			return;

		float angle = in.angle * in.side;
		state->abs_yaw = math::normalize(player->eye_angles().y + angle);
	}

	void init(c_csplayer* player, record_t* record, resolver_info_t& info)
	{
		// don't need to resolver when local is dead
		if (!g::local() || !g::local()->is_alive())
		{
			info.angle = 0.f;
			info.side = 0;
			info.got = false;
			return;
		}

		auto state = player->get_animation_state();
		if (!state)
			return;

		player_info_t p_info = {};
		i::engine->get_player_info(player->index(), &p_info);

		// detect if enemy triggers sidemove for desync
		if (player->velocity().length(true) < 0.1f)
		{
			// feet cycle changes changes only when enemy is moving / accelerating
			// when enemy is standing cycle stops his calculations
			if (state->primary_cycle != info.old_feet_cycle)
			{
				info.cycle_change_time = i::global_vars->cur_time;
				info.old_feet_cycle = state->primary_cycle;
			}

			info.no_sidemove = std::abs(i::global_vars->cur_time - info.cycle_change_time) > 0.1f;
		}

		if (info.no_sidemove || p_info.fake_player)
		{
			info.angle = 0.f;
			info.side = 0;
			info.got = false;
			return;
		}

		// we can't find something from game data
		if (info.got_layer)
			animation_resolver(player, record, info);

		info.got = true;
	}
}
