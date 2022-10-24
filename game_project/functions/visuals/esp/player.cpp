#include "player.h"

#include "../visuals.h"

#include "../../config_vars.h"

#include "../../../base/sdk.h"
#include "../../../base/global_variables.h"

#include "../../listeners/listener_entity.h"

#include "../../../base/tools/render.h"

#include "../../../base/other/game_functions.h"

#include "../../../base/ingame objects/c_csplayer.h"
#include "../../../base/ingame objects/c_basecombatweapon.h"
#include "../../../base/ingame objects/c_csplayerresource.h"

box_t boxes[64] = {};

// thanks to https://www.unknowncheats.me/forum/counterstrike-global-offensive/282111-offscreen-esp.html

void rotate_triangle_points(vector2d points[3], float rotation)
{
	const auto points_center = (points[0] + points[1] + points[2]) / vector2d(3.f, 3.f);
	for (int i = 0; i < 3; i++)
	{
		vector2d& point = points[i];

		point -= points_center;

		const auto temp_x = point.x;
		const auto temp_y = point.y;

		const auto theta = rotation;
		const auto c = std::cos(theta);
		const auto s = std::sin(theta);

		point.x = temp_x * c - temp_y * s;
		point.y = temp_x * s + temp_y * c;

		point += points_center;
	}
}

namespace player
{
	bool finished = false;
	float arrow_alpha = 0.f;

	void render_out_of_fov_arrows(c_csplayer* player)
	{
		if (!g_cfg.visuals.oof_arrow)
			return;

		vector3d view_angle = {};
		i::engine->get_view_angles(view_angle);

		vector3d local_origin = g::local()->get_abs_origin();

		const vector3d& origin = player->get_abs_origin();

		const float distance_max = 300.f;
		const float distance = local_origin.distance_to_vector(origin);

		const int arrow_size = 15;
		const float arrow_distance = 250.f;

		const float rotation = math::deg_to_rad(view_angle.y - math::angle_from_vectors(local_origin, origin).y - 90.f);

		vector2d center = vector2d(render::screen.w / 2, render::screen.h / 2);

		vector2d position =
		{
			center.x + arrow_distance * std::cos(rotation),
			center.y + arrow_distance * std::sin(rotation)
		};

		vector2d points[3] =
		{
			vector2d(position.x - arrow_size, position.y - arrow_size),
			vector2d(position.x + arrow_size, position.y),
			vector2d(position.x - arrow_size, position.y + arrow_size)
		};

		rotate_triangle_points(points, rotation);

		render::triangle_filled(points[0], points[1], points[2], g_cfg.visuals.oof_arrow_clr.manage_alpha(arrow_alpha));
		render::triangle(points[0], points[1], points[2], color(0, 0, 0, arrow_alpha));
	}

	void render_ammo(c_csplayer* player, const box_t& box)
	{
		if (!g_cfg.visuals.ammo)
			return;

		auto weapon = player->get_active_weapon();
		if (!weapon)
			return;

		auto weapon_info = weapon->get_weapon_info();
		if (!weapon_info)
			return;

		float ammo_w = (box.w * weapon->current_ammo()) / weapon_info->max_ammo_1;

		render::rect_filled(box.x - 1, box.y - 6, box.w + 2, 4, color(30, 30, 30, 200));
		render::rect_filled(box.x, box.y - 5, ammo_w, 2, g_cfg.visuals.ammo_clr);
	}

	void render_weapon_name(c_csplayer* player, const box_t& box)
	{
		if (!g_cfg.visuals.weapon)
			return;

		auto weapon = player->get_active_weapon();
		if (!weapon)
			return;

		auto weapon_info = weapon->get_weapon_info();

		if (!weapon_info)
			return;

		int add = g_cfg.visuals.name ? 14 : 0;

		std::string name = weapon_info->hud_name + 13;
		std::transform(name.begin(), name.end(), name.begin(), toupper);

		render::text(box.x + box.w / 2.f, box.y + box.h + add, g_cfg.visuals.weapon_clr,
			render::fonts::visuals, centered_x, name);
	}

	void render_health(c_csplayer* player, const box_t& box)
	{
		if (!g_cfg.visuals.hp)
			return;

		const int& hp = player->health();

		float health_w = (box.w * hp) / 100.f;

		float hp_r = 188.f;
		float hp_g = (240.f * hp) / 100.f;
		float hp_b = (2.f * hp) / 100.f;

		int add = g_cfg.visuals.ammo ? 10 : 5;

		render::rect_filled(box.x - 1, box.y - (add + 1), box.w + 2, 4, color(30, 30, 30, 200));
		render::rect_filled(box.x, box.y - add, health_w, 2, color(hp_r, hp_g, hp_b, 230));
	}

	void render_name(c_csplayer* player, const box_t& box)
	{
		if (!g_cfg.visuals.name)
			return;

		std::string player_name = game_fn::get_name(player);
		std::transform(player_name.begin(), player_name.end(), player_name.begin(), toupper);

		render::text(box.x + box.w / 2.f, box.y + box.h, g_cfg.visuals.name_clr,
			render::fonts::visuals_name, centered_x, player_name);
	}

	void render_box(const box_t& box)
	{
		if (!g_cfg.visuals.box)
			return;

		render::rect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, color(10, 10, 10, 200));
		render::rect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, color(10, 10, 10, 200));
		render::rect(box.x, box.y, box.w, box.h, g_cfg.visuals.box_clr);
	}

	void render_flags(c_csplayer* player, const box_t& box)
	{
		int flags = g_cfg.visuals.flags;

		auto _x = box.x + box.w + 3, _y = box.y - 2;

		auto draw_flag = [&](color color, const std::string& text) -> void
		{
			render::text(_x, _y, color, render::fonts::visuals_flags, none, text);
			_y += 8;
		};

		if ((flags & 1))
			draw_flag(color(150, 200, 60, 255), std::to_string(player->money()) + "$");

		if ((flags & 2) && player->armor() > 0.f)
			draw_flag(color(255, 255, 255, 255), (player->has_helmet() ? "HK" : "K"));

		if ((flags & 4) && player->is_scoped())
			draw_flag(color(60, 180, 225, 255), "ZOOM");

		if ((flags & 8) && player->flash_time() > 0.f)
			draw_flag(color(255, 255, 0, 255), "FLASH");

		if ((flags & 16) && player->have_defuser())
			draw_flag(color(240, 240, 240, 255), "KIT");

		if ((flags & 32) && player->is_defusing())
			draw_flag(color(60, 180, 225, 255), "DEFUSING");

		if ((flags & 64) && i::player_resource && player->index() == i::player_resource->c4_carrier())
			draw_flag(color(244, 65, 65, 255), "BOMB");

		//draw_flag(color(255, 255, 255, 255), "ping: " + std::to_string(interfaces::player_resource->get_ping(player->index())));
	}

	void render()
	{
		if (!g::in_game)
			return;

		if (!g_cfg.visuals.player)
			return;

		const float add_time = 255.f / 1.5f * i::global_vars->frame_time;
		if (arrow_alpha <= 255.f && !finished)
		{
			arrow_alpha += add_time;
			if (arrow_alpha >= 255.f)
				finished = true;
		}
		else if (arrow_alpha >= 0.f && finished)
		{
			arrow_alpha = 0.f;
			finished = false;
		}
		arrow_alpha = std::clamp(arrow_alpha, 0.f, 255.f);

		auto player_array = listener_entity->get_entity(ent_player);
		if (player_array.empty())
			return;

		for (auto player_info : player_array)
		{
			auto player = (c_csplayer*)player_info.entity;
			if (!player)
				continue;

			if (player->dormant() || !player->is_alive())
				continue;

			if (player == g::local() || player->team() == g::local()->team())
				continue;

			box_t& box = boxes[player->index()];
			box.get_box_bounds(player);

			if (box.valid)
			{
				render_box(box);
				render_flags(player, box);
				render_name(player, box);
				render_health(player, box);
				render_weapon_name(player, box);
				render_ammo(player, box);
			}
			else
				render_out_of_fov_arrows(player);
		}
	}
}