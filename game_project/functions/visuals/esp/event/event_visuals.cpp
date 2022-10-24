#include "event_visuals.h"

#include "../../../config_vars.h"

#include "../../../../base/sdk.h"
#include "../../../../base/global_variables.h"

#include "../../../../base/tools/math.h"
#include "../../../../base/tools/render.h"

#include "../../../../base/ingame objects/c_csplayer.h"
#include "../../../../base/ingame objects/c_basecombatweapon.h"

#define duration 0.5f

namespace visuals
{
	namespace events
	{
		namespace hit
		{
			struct client_verify_t
			{
				vector3d pos;
				float time;
				float expires;
			};

			struct hit_marker
			{
				float hurt_time = FLT_MIN;
				color hurt_color = color(255, 255, 255);
				vector3d point = vector3d(0.0f, 0.0f, 0.0f);
			} hit_marker;

			c_game_event* ptr = nullptr;

			float hit_time = 0.f;
			int damage = 0;

			void reset_info()
			{
				damage = 0;
				hit_time = 0.f;
			}

			void player_hurt()
			{
				const char* event_name = ptr->get_name();
				if (strcmp(event_name, "player_hurt"))
					return;

				if (!g_cfg.misc.hitmarker && !g_cfg.misc.damage)
				{
					reset_info();
					return;
				}

				int attacker = i::engine->get_player_for_user_id(ptr->get_int("attacker"));

				if (g::local()->index() != attacker)
					return;

				int user_id = i::engine->get_player_for_user_id(ptr->get_int("userid"));

				c_csplayer* player = (c_csplayer*)i::entity_list->get_entity(user_id);
				if (!player)
					return;

				if (g::local() == player
					|| player->team() == g::local()->team())
					return;

				hit_time = i::global_vars->cur_time;

				if (g_cfg.misc.hitmarker && g_cfg.misc.sound)
					i::surface->play_sound("buttons\\arena_switch_press_02.wav");

				if (g_cfg.misc.damage)
					damage = ptr->get_int("dmg_health");
			}

			int last_count = 0;

			void client_impact(int stage)
			{
				if (stage != frame_render_start)
					return;

				if (!g::local() || !g::local()->is_alive())
					return;

				auto& impact_list = *(c_utlvector<client_verify_t>*)((uintptr_t)g::local() + 0x11C50);

				if (g_cfg.misc.impacts)
				{
					const auto& size = g_cfg.misc.impact_size * 0.1f;
					for (auto i = impact_list.count(); i > last_count; i--)
					{
						i::debug_overlay->add_box_overlay(
							impact_list[i - 1].pos, 
							vector3d(-size, -size, -size),
							vector3d(size, size, size),
							vector3d(0, 0, 0),
							255, 0, 0, 127, 4);
					}

					if (impact_list.count() != last_count)
						last_count = impact_list.count();

					impact_list.remove_all();
				}
				else
					last_count = 0;
			}

			void bullet_impact()
			{
				const char* event_name = ptr->get_name();
				if (strcmp(event_name, "bullet_impact"))
					return;

				int user_id = i::engine->get_player_for_user_id(ptr->get_int("userid"));

				c_csplayer* player = (c_csplayer*)i::entity_list->get_entity(user_id);

				if (!player)
					return;

				if (player->dormant())
					return;

				int x, y, z;
				x = ptr->get_float("x");
				y = ptr->get_float("y");
				z = ptr->get_float("z");

				if (player == g::local())
				{
					if (g_cfg.misc.impacts)
					{
						const auto& size = g_cfg.misc.impact_size * 0.1f;
						i::debug_overlay->add_box_overlay(
							vector3d(x, y, z), vector3d(-size, -size, -size),
							vector3d(size, size, size), vector3d(0, 0, 0),
							0, 0, 255, 127, 4.f);
					}
				}

				if (g_cfg.misc.tracers)
				{
					auto final_end = vector3d(x, y, z);

					if (player->team() != g::local()->team())
						beams::draw_beam(player->bone_position(), final_end, g_cfg.misc.trace_clr[0]);

					if (player == g::local())
						beams::draw_beam(player->bone_position(), final_end, g_cfg.misc.trace_clr[1]);
				}
			}

			void parse(c_game_event* event)
			{
				if (!g::in_game)
					return;

				if (!g::local())
					return;

				ptr = event;
				player_hurt();
				bullet_impact();
			}

			void hitmarker(const float& animation_time)
			{
				if (!g_cfg.misc.hitmarker)
					return;

				float alpha = 255.f * (1.f - animation_time);

				vector2d center = vector2d(render::screen.w / 2.f, render::screen.h / 2.f);

				color hit_color = color(255, 0, 0, alpha);

				render::line(center.x - 5, center.y - 5, center.x - 10, center.y - 10, hit_color);
				render::line(center.x + 5, center.y + 5, center.x + 10, center.y + 10, hit_color);
				render::line(center.x - 5, center.y + 5, center.x - 10, center.y + 10, hit_color);
				render::line(center.x + 5, center.y - 5, center.x + 10, center.y - 10, hit_color);
			}

			void damage_marker(const float& animation_time)
			{
				if (!g_cfg.misc.damage)
					return;

				float easing = 1.f - pow(1.f - animation_time, 5.f);

				float alpha = 255.f * (1.f - animation_time);

				vector2d center = vector2d(render::screen.w / 2.f, render::screen.h / 2.f);

				color hit_color = color(255, 255, 255, alpha);

				render::text(center.x, center.y + 25.f + (50.f * easing), hit_color,
					render::fonts::visuals_large, centered_x, "-" + std::to_string(damage) + "hp");
			}

			void render()
			{
				if (!g::in_game)
					return;

				if (hit_time == 0.f)
					return;

				float animation_time = (i::global_vars->cur_time - hit_time) / duration;
				if (animation_time > 1.f)
					return;

				hitmarker(animation_time);
				damage_marker(animation_time);
			}
		}
	}
}