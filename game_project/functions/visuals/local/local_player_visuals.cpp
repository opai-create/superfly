#include "local_player_visuals.h"

#include "../visuals.h"

#include "../../config_vars.h"

#include "../../../base/sdk.h"

#include "../../listeners/listener_entity.h"

#include "../../../base/tools/render.h"

#include "../../../base/other/game_functions.h"

#include "../../../base/ingame objects/c_csplayer.h"
#include "../../../base/ingame objects/c_basecombatweapon.h"

#include <algorithm>

#undef local

namespace visuals
{
	namespace local
	{
		c_view_setup* setup_ptr = nullptr;

		void draw_debug_angle()
		{
			if (!g::cmd)
				return;

			vector3d src = g::local()->origin();
			vector3d forward = {};
			vector2d sc1, sc2;

			math::angle_to_vectors(vector3d(0, g::base_angle.y, 0), forward);
			if (render::world_to_screen(src, sc1) && render::world_to_screen(src + (forward * 40.f), sc2))
				render::line(sc1.x, sc1.y, sc2.x, sc2.y, color(255, 0, 0));
		}

		void draw_scope_lines()
		{
			if (!(g_cfg.misc.removals & scope))
				return;

			if (!g::local()->is_scoped())
				return;

			render::line(0, render::screen.h / 2, render::screen.w, render::screen.h / 2, color(0, 0, 0, 255));
			render::line(render::screen.w / 2, 0, render::screen.w / 2, render::screen.h, color(0, 0, 0, 255));
		}

		void change_fov()
		{
			// TO-DO: iron-sight animation for ssg / aug

			// our base fov
			float fov = 90.f + g_cfg.misc.fovs[world];

			if (g::local()->is_alive())
			{
				c_basecombatweapon* weapon = g::weapon();
				if (!weapon)
					return;

				int zoom_level = weapon->zoom_level();

				// we calc difference between in-game fov and custom
				// and then we add it to current fov and calc it in percentage

				float zoom_fov = weapon->get_zoom_fov(zoom_level) + g_cfg.misc.fovs[world];
				float fov_delta = fov - zoom_fov;
				float total_fov = fov_delta * (1.f - g_cfg.misc.fovs[zoom] * 0.01f);

				if (g::local()->is_scoped() && zoom_level > 0)
					fov = zoom_fov + total_fov;
			}

			setup_ptr->fov = fov;
		}

		bool should_override = false;

		void disable_post_processing()
		{
			if (should_override != (g_cfg.misc.removals & post_process))
			{
				*game_fn::override_processing = (g_cfg.misc.removals & post_process) ? true : false;
				should_override = g_cfg.misc.removals & post_process;
			}
		}

		void remove_flash()
		{
			if (!g::local()->is_alive())
				return;

			if (!(g_cfg.misc.removals & flash))
			{
				g::local()->flash_max_alpha() = 255.f;
				return;
			}

			g::local()->flash_max_alpha() = -1.f;
		}

		bool override_smoke = false;

		void remove_smoke()
		{
			// hard code but works very good (was made by myself in weave)
			static auto mat1 = i::material_system->find_material("particle/vistasmokev1/vistasmokev1_smokegrenade", "Other textures");
			static auto mat2 = i::material_system->find_material("particle/vistasmokev1/vistasmokev1_emods", "Other textures");
			static auto mat3 = i::material_system->find_material("particle/vistasmokev1/vistasmokev1_emods_impactdust", "Other textures");
			static auto mat4 = i::material_system->find_material("particle/vistasmokev1/vistasmokev1_fire", "Other textures");

			mat1->set_material_var_flag(material_var_no_draw, g_cfg.misc.removals & smoke);
			mat2->set_material_var_flag(material_var_no_draw, g_cfg.misc.removals & smoke);
			mat3->set_material_var_flag(material_var_no_draw, g_cfg.misc.removals & smoke);
			mat4->set_material_var_flag(material_var_no_draw, g_cfg.misc.removals & smoke);

			if (g_cfg.misc.removals & smoke)
				*(int*)game_fn::smoke_count = 0;
		}

		bool thirdperson_pressed = false;

		void thirdperson()
		{
			if (!g::in_game)
				return;

			bool alive = g::local() && g::local()->is_alive();

			if (g_cfg.binds[tp_b].toggled)
			{
				if (alive && !i::input->camera_in_third_person)
					i::input->camera_in_third_person = true;

				else if (g::local()->obs_server_mode() == 4)
				{
					if (i::input->camera_in_third_person)
					{
						i::input->camera_in_third_person = false;
						i::input->camera_offset.z = 0.f;
					}

					g::local()->obs_server_mode() = 5;
				}

				thirdperson_pressed = true;
			}
			else if (i::input->camera_in_third_person && thirdperson_pressed)
			{
				i::input->camera_in_third_person = false;
				i::input->camera_offset.z = 0.f;
				thirdperson_pressed = false;
			}

			if (i::input->camera_in_third_person)
			{
				auto distance = g_cfg.misc.thirdperson_dist;

				vector3d angles = {};
				i::engine->get_view_angles(angles);

				vector3d inverse_angles = {};
				i::engine->get_view_angles(inverse_angles);

				inverse_angles.z = distance;

				vector3d forward, right, up = {};
				math::angle_to_vectors(inverse_angles, &forward, &right, &up);

				c_trace_filter filter(g::local());
				c_game_trace trace = {};

				auto eye_pos = g_cfg.binds[fd_b].toggled
					? g::local()->get_render_origin() + i::game_movement->get_player_view_offset(false)
					: g::local()->get_abs_origin() + g::local()->view_offset();

				auto offset = eye_pos + forward * -distance + right + up;
				i::engine_trace->trace_ray(ray_t(eye_pos, offset, vector3d(-16.f, -16.f, -16.f), vector3d(16.f, 16.f, 16.f)), mask_shot_hull, &filter, &trace);

				trace.fraction = std::clamp(trace.fraction, 0.f, 1.f);

				float out_fr = trace.entity && trace.entity->is_player() ? 1.f : trace.fraction;
				angles.z = g_cfg.misc.thirdperson_dist * out_fr;

				i::input->camera_offset = angles;
			}
		}

		void draw_pt()
		{
			if (!g::in_game)
				return;

			if (!g::local())
				return;

			if (!g::local()->is_alive())
				return;

			draw_scope_lines();
			//draw_debug_angle();
		}

		void change_view(c_view_setup* setup)
		{
			if (!g::in_game)
				return;

			if (!g::local())
				return;

			setup_ptr = setup;
			change_fov();
			thirdperson();
		}

		void view_after_orig()
		{
			if (!g::in_game)
				return;

			if (!g::local())
				return;

			if (!g::local()->is_alive())
				return;

			if (g_cfg.binds[fd_b].toggled)
			{
				setup_ptr->origin = g::local()->get_render_origin() + vector3d(0.0f, 0.0f, i::game_movement->get_player_view_offset(false).z + 0.064f);

				if (i::input->camera_in_third_person)
				{
					vector3d angles = vector3d(i::input->camera_offset.x, i::input->camera_offset.y, 0.f);

					vector3d forward = {};
					math::angle_to_vectors(angles, forward);

					math::vector_multiply(setup_ptr->origin, -i::input->camera_offset.z, forward, setup_ptr->origin);
				}
			}
		}

		void draw_fsn(int stage)
		{
			if (!g::in_game)
				return;

			if (!g::local())
				return;

			if (stage != frame_render_start)
				return;

			disable_post_processing();
			remove_smoke();
			remove_flash();
		}
	}
}