#include "nades.h"

#include "../visuals.h"

#include "../../config_vars.h"

#include "../../../base/sdk.h"

#include "../../listeners/listener_entity.h"

#include "../../../base/tools/render.h"

#include "../../../base/other/game_functions.h"

#include "../../../base/ingame objects/c_csplayer.h"
#include "../../../base/ingame objects/c_basecombatweapon.h"

const char* get_name_by_class_id(const model_t& model, const int& class_id)
{
	switch (class_id)
	{
	case (int)CBaseCSGrenadeProjectile:
		return strstr(model.name, "flashbang") ? "FLASH" : "HE GRENADE";
		break;
	case (int)CBreachChargeProjectile:
		return "BREACH";
		break;
	case (int)CBumpMineProjectile:
		return "MINE";
		break;
	case (int)CDecoyProjectile:
		return "DECOY";
		break;
	case (int)CMolotovProjectile:
		return "FIRE";
		break;
	case (int)CSensorGrenadeProjectile:
		return "SENSOR";
		break;
	case (int)CSmokeGrenadeProjectile:
		return "SMOKE";
		break;
	case (int)CSnowballProjectile:
		return "SNOW";
		break;
	}
	return "UNK";
}

namespace nades
{
	const vector2d timer_size = vector2d(60.f, 2.f);

	void render_fire_radius(c_baseentity* entity)
	{
		if (!(g_cfg.visuals.world[nade].timer & 1))
			return;

		vector3d origin = entity->get_abs_origin();

		const float fire_duration = 7.03125f;
		const float expire_time = (((*(float*)(uintptr_t(entity) + 0x20)) + fire_duration) - i::global_vars->cur_time);

		vector2d screen_origin = {};
		if (render::world_to_screen(origin, screen_origin))
		{
			vector2d total_pos = vector2d(screen_origin.x - timer_size.x / 2, screen_origin.y - timer_size.y);
			render::rect_filled(total_pos.x - 1, total_pos.y - 1, timer_size.x + 2, timer_size.y + 2, color(30, 30, 30, 200));
			render::rect_filled(total_pos.x, total_pos.y, (timer_size.x * expire_time) / fire_duration, timer_size.y, color(250, 70, 70, 200));

			render::text(screen_origin.x, screen_origin.y, color(), render::fonts::visuals, centered_x, "FIRE");
		}
	}

	void render_smoke_radius(c_baseentity* entity)
	{
		if (!(g_cfg.visuals.world[nade].timer & 2))
			return;

		vector3d origin = entity->get_abs_origin();

		const float range = 120.f;
		const float tick_time = (entity->smoke_effect_tick_begin() * i::global_vars->interval_per_tick) + 17.5f;
		const float expire_time = tick_time - i::global_vars->cur_time;

		vector2d screen_origin = {};
		if (render::world_to_screen(origin, screen_origin))
		{
			vector2d total_pos = vector2d(screen_origin.x - timer_size.x / 2, screen_origin.y - timer_size.y);
			render::rect_filled(total_pos.x - 1, total_pos.y - 1, timer_size.x + 2, timer_size.y + 2, color(30, 30, 30, 200));
			render::rect_filled(total_pos.x, total_pos.y, (timer_size.x * expire_time) / 18.f, timer_size.y, color(51, 170, 255, 200));

			render::text(screen_origin.x, screen_origin.y, color(), render::fonts::visuals, centered_x, "SMOKE");
		}
	}

	void render_name(c_baseentity* entity, const box_t& box)
	{
		if (!g_cfg.visuals.world[nade].name)
			return;

		c_client_class* client_class = entity->get_client_class();
		if (!client_class)
			return;

		model_t* model = entity->get_model();
		if (!model)
			return;

		int class_id = client_class->class_id;

		render::text(box.x + box.w / 2, box.y + box.h, g_cfg.visuals.world[nade].name_clr, render::fonts::visuals,
			centered_x, get_name_by_class_id(*model, class_id));
	}

	void render_box(c_baseentity* entity, const box_t& box)
	{
		if (!g_cfg.visuals.world[nade].box)
			return;

		render::rect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, color(30, 30, 30, 200));
		render::rect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, color(30, 30, 30, 200));
		render::rect(box.x, box.y, box.w, box.h, g_cfg.visuals.world[nade].box_clr);
	}

	void render()
	{
		if (!g::in_game)
			return;

		if (!g_cfg.visuals.world[nade].enable)
			return;

		auto grenade_array = listener_entity->get_entity(ent_grenade);
		if (grenade_array.empty())
			return;

		for (auto grenade_info : grenade_array)
		{
			auto grenade = grenade_info.entity;
			if (!grenade)
				continue;

			int class_id = grenade_info.class_id;
			if (class_id == CInferno)
				render_fire_radius(grenade);
			if (class_id == CSmokeGrenadeProjectile && grenade->did_smoke_effect())
				render_smoke_radius(grenade);

			if (grenade->nade_exploded()
				|| class_id == CSmokeGrenadeProjectile && grenade->did_smoke_effect())
				continue;

			box_t box(grenade);
			if (box.valid)
			{
				render_box(grenade, box);
				render_name(grenade, box);
			}
		}
	}
}