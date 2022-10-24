#include "ragebot.h"
#include "autowall.h"

#include "../../additionals/threading/threading.h"

#include "../config_vars.h"
#include "../../base/global_variables.h"

#include "../extra/function_checks.h"
#include "../listeners/listener_entity.h"

#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

#include "../hvh/resolver.h"
#include "../anti hit/exploits.h"

#include "../extra/movement.h"

#include <vector>

namespace aim_utils
{
	const int TRACE_AMT = 48;

	const float HEAD_SCALE = 1.f;
	const float BODY_SCALE = 1.f;

	vector3d get_recoil_angle()
	{
		static auto weapon_recoil_scale = i::convar->find_convar("weapon_recoil_scale");
		if (!weapon_recoil_scale)
			return vector3d();

		return g::local()->aim_punch_angle() * weapon_recoil_scale->get_float();
	}

	bool is_nospread()
	{
		static auto weapon_accuracy_nospread = i::convar->find_convar("weapon_accuracy_nospread");
		if (weapon_accuracy_nospread && weapon_accuracy_nospread->get_int() > 0)
			return true;

		return false;
	}

	bool can_hit_hitbox(const vector3d& start, const vector3d& end, c_csplayer* player, int hitbox, matrix3x4_t* matrix)
	{
		auto model = i::model_info->get_studio_model(player->get_model());
		auto set = model->get_hitbox_set(0);

		if (!set)
			return false;

		auto studio_box = set->get_hitbox(hitbox);
		if (!studio_box)
			return false;

		vector3d min, max;

		const auto is_capsule = studio_box->radius != -1.f;

		if (is_capsule)
		{
			math::vector_transform(studio_box->bbmin, matrix[studio_box->bone], min);
			math::vector_transform(studio_box->bbmax, matrix[studio_box->bone], max);

			const auto dist = math::segment_to_segment(start, end, min, max);

			if (dist < studio_box->radius)
				return true;
		}
		else
		{
			math::vector_transform(math::vector_rotate(studio_box->bbmin, studio_box->rotation), matrix[studio_box->bone], min);
			math::vector_transform(math::vector_rotate(studio_box->bbmax, studio_box->rotation), matrix[studio_box->bone], max);

			math::vector_i_transform(start, matrix[studio_box->bone], min);
			math::vector_i_rotate(end, matrix[studio_box->bone], max);

			if (math::intersect_line_with_bb(min, max, studio_box->bbmin, studio_box->bbmax))
				return true;
		}

		return false;
	}

	bool hit(best_target_t& best, float chance)
	{
		if (is_nospread())
			return true;

		auto weapon = g::weapon();
		if (!weapon)
			return false;

		auto weapon_info = weapon->get_weapon_info();
		if (!weapon_info)
			return false;

		vector3d forward, right, up;
		vector3d src = g::local()->eye_pos();
		vector3d pos = math::angle_from_vectors(src, best.point);
		math::angle_to_vectors(pos, &forward, &right, &up);

		auto damage = 0;
		auto hits = 0;
		float weap_spread = g::weapon()->get_spread();
		float weap_inaccuracy = g::weapon()->get_inaccuracy();

		auto spread = weap_spread + weap_inaccuracy;

		for (auto i = 1; i <= 6; ++i)
		{
			for (auto j = 0; j < 8; ++j)
			{
				auto current_spread = spread * ((float)i / 6.f);

				float value = (float)j / 8.0f * (M_PI * 2.f);

				auto direction_cos = std::cos(value);
				auto direction_sin = std::sin(value);

				auto spread_x = direction_sin * current_spread;
				auto spread_y = direction_cos * current_spread;

				vector3d direction = {};

				direction.x = forward.x + spread_x * right.x + spread_y * up.x;
				direction.y = forward.y + spread_x * right.y + spread_y * up.y;
				direction.z = forward.z + spread_x * right.z + spread_y * up.z;

				auto end = src + direction * weapon_info->range;

				if (can_hit_hitbox(src, end, best.record->player, best.hitbox, best.record->bones))
				{
					hits++;
	
					/*auto autowall_data = autowall::run(end, best.record->player, g::local(), src, aim_utils::hitbox_to_hitgroup(best.hitbox));
					if (autowall_data.damage >= 1)
						damage++;*/
				}
			}
		}

		if (((float)hits / (float)TRACE_AMT) >= chance
			/*&& ((float)damage / (float)TRACE_AMT) >= chance*/)
			return true;

		return false;
	}

	int hitbox_to_hitgroup(int hitbox) 
	{
		switch (hitbox)
		{
		case hitbox_head: 
		case hitbox_neck:
			return hitgroup_head;
			break;
		case hitbox_pelvis:
		case hitbox_stomach:
			return hitgroup_stomach;
			break;
		case hitbox_lower_chest:
		case hitbox_chest:
		case hitbox_upper_chest:
			return hitgroup_chest;
			break;
		case hitbox_left_thigh:
		case hitbox_left_calf:
		case hitbox_left_foot:
			return hitgroup_leftleg;
			break;
		case hitbox_right_thigh:
		case hitbox_right_calf:
		case hitbox_right_foot:
			return hitgroup_rightleg;
			break;
		case hitbox_left_hand:
		case hitbox_left_upper_arm:
		case hitbox_left_forearm:
			return hitgroup_leftarm;
			break;
		case hitbox_right_hand:
		case hitbox_right_upper_arm:
		case hitbox_right_forearm:
			return hitgroup_rightarm;
			break;

		default: 
			return hitgroup_generic;
			break;
		}
	}

	std::vector<std::pair<vector3d, bool>> multi_point(c_csplayer* player, int hitbox, matrix3x4_t* matrix)
	{
		std::vector<std::pair<vector3d, bool>> points = {};

		auto model = player->get_model();
		if (!model)
			return points;

		auto hdr = i::model_info->get_studio_model(model);
		if (!hdr)
			return points;

		auto set = hdr->get_hitbox_set(0);
		if (!set)
			return points;

		auto bbox = set->get_hitbox(hitbox);
		if (!bbox)
			return points;

		// these indexes represent boxes.
		if (bbox->radius <= 0.f) 
		{
			// references: 
			//      https://developer.valvesoftware.com/wiki/Rotation_Tutorial
			//      CBaseAnimating::GetHitboxBonePosition
			//      CBaseAnimating::DrawServerHitboxes

			// convert rotation angle to a matrix.
			matrix3x4_t rot_matrix = {};
			rot_matrix.angle_matrix(bbox->rotation);

			// apply the rotation to the entity input space (local).
			matrix3x4_t mat = {};
			math::contact_transforms(matrix[bbox->bone], rot_matrix, mat);

			// extract origin from matrix.
			vector3d origin = mat.get_origin();

			// compute raw center point.
			vector3d center = (bbox->bbmin + bbox->bbmax) * 0.5f;

			// add center point

			if (hitbox == hitbox_left_foot || hitbox == hitbox_right_foot)
				points.emplace_back(center, true);

			// nothing to do here we are done.
			if (points.empty())
				return points;

			// rotate our bbox points by their correct angle
			// and convert our points to world space.
			for (auto& p : points)
			{
				// VectorRotate.
				// rotate point by angle stored in matrix.
				p.first = { p.first.dot(mat.mat[0]), p.first.dot(mat.mat[1]), p.first.dot(mat.mat[2]) };

				// transform point to world space.
				p.first += origin;
			}
		}
		// these hitboxes are capsules.
		else 
		{

			// compute raw center point.
			vector3d max = bbox->bbmax;
			vector3d min = bbox->bbmin;
			vector3d center = (bbox->bbmin + bbox->bbmax) * 0.5f;

			switch (hitbox)
			{
			case hitbox_head:
			{
				// add center.
				float r = bbox->radius * HEAD_SCALE;
				points.emplace_back(center, true);

				// rotation matrix 45 degrees.
				// https://math.stackexchange.com/questions/383321/rotating-x-y-points-45-degrees
				// std::cos( deg_to_rad( 45.f ) )
				constexpr float rotation = 0.70710678f;

				// top/back 45 deg.
				// this is the best spot to shoot at.
				points.emplace_back(vector3d{ max.x + (rotation * r * 0.7f), max.y + (-rotation * r * 0.7f), max.z }, false);

				// right.
				points.emplace_back(vector3d{ max.x, max.y, max.z + r }, false);

				// left.
				points.emplace_back(vector3d{ max.x, max.y, max.z - r }, false);

				// back.
				points.emplace_back(vector3d{ max.x, max.y - r, max.z }, false);
			}
			break;
			case hitbox_stomach:
			{
				float r = bbox->radius * BODY_SCALE;

				// center.
				points.emplace_back(center, true);
				points.emplace_back(vector3d(center.x, center.y, min.z + r), false);
				points.emplace_back(vector3d(center.x, center.y, max.z - r), false);
				// back.
				points.emplace_back(vector3d{ center.x, max.y - r, center.z }, true);
			}
			break;

			case hitbox_lower_chest:
			case hitbox_pelvis:
			case hitbox_chest:
			{
				float r = bbox->radius;
				points.emplace_back(center, true);

				// left & right points
				points.emplace_back(vector3d(center.x, center.y, bbox->bbmax.z + bbox->radius), false);
				points.emplace_back(vector3d(center.x, center.y, bbox->bbmin.z - bbox->radius), false);

				// add extra point on back.
				points.emplace_back(vector3d{ center.x, bbox->bbmax.y - bbox->radius, center.z }, false);
			}
			break;
			case hitbox_left_calf:
			case hitbox_right_calf:
				points.emplace_back(center, true);
				break;
			case hitbox_left_thigh:
			case hitbox_right_thigh:
			{
				// add center.
				points.emplace_back(center, true);

				// bottom point (knees)
				points.emplace_back(vector3d{ max.x + (bbox->radius / 3.f), max.y, max.z }, false);
			}
			break;
			case hitbox_left_upper_arm:
			case hitbox_right_upper_arm:
			{
				// elbow.
				points.emplace_back(vector3d{ max.x + (bbox->radius / 2.f), center.y, center.z }, false);
			}
			break;
			}

			// nothing left to do here.
			if (points.empty())
				return points;

			// transform capsule points.
			for (auto& p : points)
				math::vector_transform(p.first, matrix[bbox->bone], p.first);
		}

		return points;
	}
}

namespace ragebot
{
	struct restore_t
	{
		float duck = -1.f;

		vector3d origin = {};
		vector3d velocity = {};
		vector3d mins = {};
		vector3d maxs = {};
		alignas(16) matrix3x4_t bone_cache[256] = {};
	};

	std::array<restore_t, 65> restore_data = {};

	std::vector<c_csplayer*> players = {};

	std::array<int, 13> hitbox_list =
	{
		hitbox_head,
		hitbox_chest,
		hitbox_lower_chest,
		hitbox_stomach,
		hitbox_pelvis,
		hitbox_left_upper_arm,
		hitbox_right_upper_arm,
		hitbox_left_thigh,
		hitbox_right_thigh,
		hitbox_left_calf,
		hitbox_right_calf,
		hitbox_left_foot,
		hitbox_right_foot,
	};

	void store(c_csplayer* player)
	{
		auto idx = player->index();

		auto& d = restore_data[idx];

		d.origin = player->origin();
		d.velocity = player->velocity();
		d.duck = player->duck_amount();
		d.mins = player->mins();
		d.maxs = player->maxs();
		std::memcpy(d.bone_cache, player->get_bone_cache().base(), sizeof(matrix3x4_t) * player->get_bone_cache().count());
	}

	void restore(c_csplayer* player)
	{
		auto idx = player->index();

		auto& d = restore_data[idx];

		game_fn::set_abs_origin(player, d.origin);
		player->origin() = d.origin;
		player->velocity() = d.velocity;
		player->duck_amount() = d.duck;
		player->mins() = d.mins;
		player->maxs() = d.maxs;
		std::memcpy(player->get_bone_cache().base(), d.bone_cache, sizeof(matrix3x4_t) * player->get_bone_cache().count());
	}

	void set(c_csplayer* player, record_t* record)
	{
		game_fn::set_abs_origin(player, record->origin);
		player->origin() = record->origin;
		player->velocity() = record->velocity;
		player->duck_amount() = record->duck;
		player->mins() = record->obb_mins;
		player->maxs() = record->obb_maxs;
		std::memcpy(player->get_bone_cache().base(), record->bones, sizeof(matrix3x4_t) * player->get_bone_cache().count());
	}

	best_target_t best = {};

	std::array<std::array<std::vector<tick_selection_t>, 2>, 65> tick_selection = {};

	int get_total_damage(c_csplayer* player, record_t* record, int type, int idx)
	{
		tick_selection[idx][type].clear();

		set(player, record);

		int total_dmg = 0;

		for (auto h : hitbox_list)
		{
			auto pos = player->hitbox_position(h, record->bones);

			auto info = autowall->wall_penetration(g::local()->eye_pos(), pos, player);
			if (!info.visible && info.hits >= 4)
				continue;

			if (info.damage > 0)
				total_dmg += info.damage;

			tick_selection[idx][type].emplace_back(tick_selection_t(pos, info.damage, h));
		}

		restore(player);
		return total_dmg;
	}

	int get_scan_type(c_csplayer* player, record_t* last, record_t* old, int idx)
	{
		int dmg_last = get_total_damage(player, last, last_tick, idx);
		if (dmg_last <= 0)
			return -1;

		int dmg_old = -1;

		if (old)
			dmg_old = get_total_damage(player, old, old_tick, idx);

		if (dmg_old > dmg_last)
			return old_tick;

		return last_tick;
	}

	bool hitchance_zero = false;

	void scan_points()
	{
		int best_damage = INT_MIN;
		vector3d best_point = {};
		int best_hitbox = -1;

		for (auto p : players)
		{
			int idx = p->index();

			store(p);

			auto last = lag_compensation->get_latest_record(p);
			if (!last)
				continue;

			auto old = lag_compensation->get_oldest_record(p);

			int scan = get_scan_type(p, last, old, idx);
			if (scan == -1)
				continue;

			auto cur_scan = tick_selection[idx][scan];
			auto rec = scan == last_tick ? last : old;

			set(p, rec);

			std::vector<aimbot_data_t> aimbot_data = {};
			for (auto& s : cur_scan)
			{
				aimbot_data_t data = {};
				data.hitbox = s.hitbox;
				data.body = s.hitbox >= hitbox_pelvis && s.hitbox <= hitbox_chest;

				auto points = aim_utils::multi_point(p, s.hitbox, rec->bones);
				for (auto& pt : points)
				{
					returninfo_t info = {};
					bool multipoint_dmg = !pt.second || s.damage <= 0 && pt.first != s.point;

					if (multipoint_dmg)
						info = autowall->wall_penetration(g::local()->eye_pos(), pt.first, p);

					bool legs = s.hitbox == hitbox_left_foot || s.hitbox == hitbox_right_foot;
					bool invalid_dmg = legs ? 
						multipoint_dmg && info.damage < g_cfg.rage.mindamage
						: info.damage < g_cfg.rage.mindamage;

					if (s.damage < g_cfg.rage.mindamage
						|| invalid_dmg)
						continue;

					data.points[0] = s.point; 
					data.points[1] = pt.first;

					data.damages[0] = s.damage;
					data.damages[1] = info.damage;

					data.record = rec;

					aimbot_data.emplace_back(data);
				}
			}

			restore(p);

			if (aimbot_data.empty())
				continue;

			bool is_dt = g::weapon()->definition_index() != weapon_awp
				&& g::weapon()->definition_index() != weapon_ssg08
				&& g_cfg.binds[dt_b].toggled;

			for (const auto& d : aimbot_data)
			{
				/*auto res = resolver::info[idx - 1];

				auto safe_p = [&](vector3d pt)
				{
					if (res.side == 0)
						return false;

					bool hit = res.side == -1 ?
						aim_utils::can_hit_hitbox(g::local()->eye_pos(), pt, p, hitbox_head, d.record->right_bones)
						: aim_utils::can_hit_hitbox(g::local()->eye_pos(), pt, p, hitbox_head, d.record->left_bones);

					return aim_utils::can_hit_hitbox(g::local()->eye_pos(), pt, p, hitbox_head, d.record->bones) && hit;
				};*/

				bool should_baim = is_dt || p->health() < 90;

				if (d.body && should_baim)
				{
					float hp = is_dt ? p->health() * 0.5f : p->health();

					if (d.damages[0] >= (int)hp)
					{
						hitchance_zero = true;
						best.get(d.points[0], d.hitbox, d.record, d.damages[0]);
						break;
					}
					else
					{
						if (d.damages[1] > best_damage)
						{
							best.get(d.points[1], d.hitbox, d.record, d.damages[1]);
							best_damage = d.damages[1];
						}
						break;
					}
				}
				else
				{
					for (int i = 0; i < 2; i++)
					{
						if (d.damages[i] > best_damage)
						{
							best.get(d.points[i], d.hitbox, d.record, d.damages[i]);
							best_damage = d.damages[i];
						}
					}

					
				}
			}
		}
	}

	bool should_stop = false;

	bool force_accuracy = true;

	void auto_stop()
	{
		if (!should_stop)
			return;

		auto weapon = g::weapon();
		if (!weapon)
			return;

		weapon_info_t* info = g::weapon()->get_weapon_info();
		if (!info)
			return;

		auto can_stop = [&]()
		{
			if (weapon->definition_index() == weapon_awp || weapon->definition_index() == weapon_ssg08)
			{
				if (!misc_checks::is_able_to_shoot())
					return false;
			}
			return true;
		};

		if (!can_stop())
		{
			should_stop = false;
			force_accuracy = true;
			return;
		}

		float max_speed = (g::local()->is_scoped() ? info->max_speed_alt : info->max_speed) * 0.33f;
		movement::stop_on_walk(max_speed);

		should_stop = false;
	}

	void aim_at_enemy()
	{
		if (!best.filled || !best.record)
			return;
		
		auto_stop();

		auto weapon = g::weapon();
		if (!weapon)
			return;

		weapon_info_t* info = g::weapon()->get_weapon_info();
		if (!info)
			return;

		float max_speed = (g::local()->is_scoped() ? info->max_speed_alt : info->max_speed) * 0.33f;

		if (exploit[dt].enabled)
			force_accuracy = g::local()->velocity().length(true) <= max_speed;
		else force_accuracy = true;
		//log_console("damage: %d \n", best.damage);

		if ((g_cfg.rage.auto_ & 2) && (g::local()->flags() & fl_onground) && g::local()->velocity().length(true) > 5.f
			&& !g_cfg.binds[sw_b].toggled)
			should_stop = true;

		if (misc_checks::is_able_to_shoot())
		{
			if (g::weapon()->is_sniper() && !g::local()->is_scoped()
				&& (g_cfg.rage.auto_ & 4) && !(g::cmd->buttons & in_attack2))
				g::cmd->buttons |= in_attack2;

			float hitchance = hitchance_zero && exploit[dt].bullets > 0 ? 0.f : (g_cfg.rage.hitchance * 0.01f);

			if (aim_utils::hit(best, hitchance) && force_accuracy)
			{
				if (g_cfg.rage.auto_ & 1)
					g::cmd->buttons |= in_attack;

				if (misc_checks::is_firing())
				{
					if (!g_cfg.binds[fd_b].toggled)
						*g::send_packet = true;

					g::cmd->tickcount = math::time_to_ticks(best.record->sim_time + lag_compensation->get_lerp_time());

					auto point = math::angle_from_vectors(g::local()->eye_pos(), best.point);
					auto recoil = aim_utils::get_recoil_angle();

					g::cmd->viewangles = math::normalize(point - recoil);
					g::base_angle = g::cmd->viewangles;
				}
			}
		}
	}

	void get_aimbot_targets()
	{
		players.clear();

		auto player_array = listener_entity->get_entity(ent_player);
		if (player_array.empty())
			return;

		for (auto player_info : player_array)
		{
			auto player = (c_csplayer*)player_info.entity;
			if (!player)
				continue;

			if (player->dormant() || !player->is_alive() || player->have_immunity())
				continue;

			if (player == g::local() || player->team() == g::local()->team())
				continue;

			players.emplace_back(player);
		}

		std::sort(players.begin(), players.end(), [&](c_csplayer* a, c_csplayer* b)
			{
				return g::local()->origin().distance_to_vector(a->origin())
					> g::local()->origin().distance_to_vector(b->origin());
			});
	}

	void run()
	{
		if (g::weapon() && g::weapon()->is_misc_weapon())
			return;

		if (!g_cfg.rage.enable)
			return;

		best.reset();

		hitchance_zero = false;

		get_aimbot_targets();
		scan_points();
		aim_at_enemy();
	}
}