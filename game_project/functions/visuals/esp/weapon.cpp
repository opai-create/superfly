#include "weapon.h"

#include "../visuals.h"

#include "../../config_vars.h"

#include "../../../base/sdk.h"

#include "../../listeners/listener_entity.h"

#include "../../../base/tools/render.h"

#include "../../../base/other/game_functions.h"

#include "../../../base/ingame objects/c_csplayer.h"
#include "../../../base/ingame objects/c_basecombatweapon.h"

#include <string>

namespace weapon
{
	// we can't get bounds from planted bomb
	// so let's use origin of bomb to render things
	void render_planted_bomb(c_baseentity* entity, const box_t& box)
	{
		/*if (!g_cfg.visuals.world[wpn].timer)
			return;*/

		if (box.class_id != CPlantedC4)
			return;

		if (box.alpha <= 0.f)
			return;

		vector2d bomb_text_width = render::text_width(render::fonts::visuals, "C4");

		float blow_time = (entity->bomb_blow() - i::global_vars->cur_time) / entity->bomb_timer_length();

		vector3d origin = entity->get_abs_origin();

		vector2d screen_origin = {};
		if (render::world_to_screen(origin, screen_origin))
		{
			render::text(screen_origin.x, screen_origin.y,
				color(255, 0,0, box.alpha), render::fonts::visuals, centered_x, "C4");

			if (blow_time > 0.f)
			{
				int modifier = bomb_text_width.x * blow_time;

				int r = 255 - ((145 * modifier) / bomb_text_width.x);
				int g = 185 + ((70 * modifier) / bomb_text_width.x);

				render::rect_filled((screen_origin.x - bomb_text_width.x / 2) - 1,
					(screen_origin.y - bomb_text_width.y / 2) - 1,
					bomb_text_width.x + 2, 4, color(50, 50, 50, box.alpha));

				render::rect_filled(screen_origin.x - bomb_text_width.x / 2,
					screen_origin.y - bomb_text_width.y / 2,
					modifier, 2, color(r, g, 70, box.alpha));
			}
		}
	}

	void render_ammo(c_baseentity* entity, const box_t& box)
	{
		if (is_bomb(box.class_id))
			return;

		if (!g_cfg.visuals.world[wpn].ammo)
			return;

		auto weapon = (c_basecombatweapon*)entity;
		if (!weapon)
			return;

		auto weapon_info = weapon->get_weapon_info();
		if (!weapon_info)
			return;

		float ammo_w = (box.w * weapon->current_ammo()) / weapon_info->max_ammo_1;

		render::rect_filled(box.x - 1, box.y - 6, box.w + 2, 4, color(50, 50, 50, box.alpha));
		render::rect_filled(box.x, box.y - 5, ammo_w, 2, g_cfg.visuals.world[wpn].ammo_clr.manage_alpha(box.alpha));
	}

	void render_name(c_baseentity* entity, const box_t& box)
	{
		if (!g_cfg.visuals.world[wpn].name)
			return;

		auto weapon = (c_basecombatweapon*)entity;
		if (!weapon)
			return;

		auto weapon_info = weapon->get_weapon_info();
		if (!weapon_info)
			return;

		std::string weapon_name = weapon_info->hud_name;
		std::transform(weapon_name.begin(), weapon_name.end(), weapon_name.begin(), toupper);

		render::text(box.x + box.w / 2, box.y + box.h, g_cfg.visuals.world[wpn].name_clr.manage_alpha(box.alpha), render::fonts::visuals,
			centered_x, weapon_name);
	}

	void render_box(c_baseentity* entity, const box_t& box)
	{
		if (!g_cfg.visuals.world[wpn].box)
			return;

		render::rect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, color(50, 50, 50, box.alpha));
		render::rect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, color(50, 50, 50, box.alpha));
		render::rect(box.x, box.y, box.w, box.h, g_cfg.visuals.world[wpn].box_clr.manage_alpha(box.alpha));
	}

	void debug_class_id()
	{
		for (int i = 0; i < i::entity_list->get_highest_ent_index(); i++)
		{
			c_baseentity* entity = (c_baseentity*)i::entity_list->get_entity(i);
			if (!entity)
				continue;

			c_client_class* client_class = entity->get_client_class();
			vector3d origin = entity->get_abs_origin();

			vector2d screen_origin = {};
			if (render::world_to_screen(origin, screen_origin))
				render::text(screen_origin.x, screen_origin.y - 14, color(), render::fonts::visuals, centered_x, client_class->network_name);
		}
	}

	void render()
	{
		if (!g::in_game)
			return;

		if (!g_cfg.visuals.world[wpn].enable)
			return;

		const float distance_max = 1000.f;

		//debug_class_id();

		auto weapon_array = listener_entity->get_entity(ent_weapon);
		if (weapon_array.empty())
			return;

		for (auto weapon_info : weapon_array)
		{
			auto weapon = weapon_info.entity;
			if (!weapon)
				continue;

			box_t box = {};

			vector3d local_origin = g::local()->get_abs_origin();
			float distance = local_origin.distance_to_vector(weapon->get_abs_origin());

			box.alpha = 255.f - std::clamp((255.f * distance) / distance_max, 0.f, 255.f);
			box.class_id = weapon_info.class_id;

			//render_planted_bomb(weapon, box);

			int owner = weapon->owner_handle();
			if (owner > 0)
				continue;

			box.get_box_bounds(weapon);

			if (box.valid && box.alpha > 0.f && box.class_id != CPlantedC4)
			{
				render_box(weapon, box);
				render_name(weapon, box);
				render_ammo(weapon, box);
			}
		}
	}
}