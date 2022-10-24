#include "world_modulation.h"

#include "../config_vars.h"

#include "../listeners/listener_entity.h"

#include "../../base/sdk.h"

#include "../../base/other/color.h"
#include "../../base/other/game_functions.h"

#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

namespace world_modulation
{
	fog_data_t fog_data[4096] = {};

	const color& world = g_cfg.misc.world_clr[walls];
	const color& skybox = g_cfg.misc.world_clr[sky];

	void change_world_clr(c_material* ptr, float* r, float* g, float* b)
	{
		if (ptr->is_error_material())
			return;

		if (!g_cfg.misc.nightmode)
			return;

		const char* texture_group = ptr->get_texture_group_name();
		if (!strcmp(texture_group, "World textures"))
		{
			*r *= world.r() * 0.0039f;
			*g *= world.g() * 0.0039f;
			*b *= world.b() * 0.0039f;
		}

		if (!strcmp(texture_group, "SkyBox textures"))
		{
			*r *= skybox.r() * 0.0039f;
			*g *= skybox.g() * 0.0039f;
			*b *= skybox.b() * 0.0039f;
		}
	}

	void change_clr(int a2, int a3, int a4, int a5, int a6, char a7)
	{
		if (!g_cfg.misc.nightmode)
			return;

		model_list_by_type_t* model_list_array = (model_list_by_type_t*)a4;
		for (int i = 0; i < a3; ++i)
		{
			model_list_by_type_t& list = model_list_array[i];
			if (!list.count)
				continue;

			for (int j = 0; j < list.count; ++j)
			{
				render_model_info_t* model = &list.render_models[j];

				c_renderable* renderable = (c_renderable*)model->entry.renderable;

				c_baseentity* entity = (c_baseentity*)renderable->get_i_unknown_entity()->get_base_entity();
				if (entity)
				{
					int class_id = entity->get_client_class()->class_id;

					auto valid_entity = [&]
					{
						switch (class_id)
						{
						case CCSRagdoll:
						case CChicken:
						case CHostage:
						case CHostageCarriableProp:
							return false;
							break;
						}
						return true;
					};

					if (!valid_entity())
						continue;
				}

				model->diffuse_modulation = g_cfg.misc.world_clr[props].as_vector_4d();
			}
		}
	}

	std::string prev_sky = "";
	int backup_skybox_area = -1;
	void change_skybox()
	{
		if (g_cfg.misc.nightmode && prev_sky != "space_14")
		{
			game_fn::load_named_sky("space_14");
			prev_sky = "space_14";
		}
		else if (!g_cfg.misc.nightmode && prev_sky != g::sky_name)
		{
			game_fn::load_named_sky(g::sky_name.c_str());
			prev_sky = g::sky_name;
		}

		// disable 3d skybox render
		if (!g_cfg.misc.nightmode && backup_skybox_area == -1)
			backup_skybox_area = g::local()->get_skybox_area();

		g::local()->get_skybox_area() = g_cfg.misc.nightmode ? 255 : backup_skybox_area;
	}

	float calculate_fog_end()
	{
		if (!g_cfg.misc.dynamic_fog)
			return 9500.f;

		vector3d head = g::local()->get_abs_origin() + g::local()->view_offset();

		vector3d view_angles = {};
		i::engine->get_view_angles(view_angles);

		vector3d world_vector = {};
		math::angle_to_vectors(view_angles, world_vector);

		ray_t ray = {};
		ray.init(head, head + world_vector * 8192.f);

		c_trace_filter filter{};
		filter.skip_entity = g::local();

		c_game_trace out_trace = {};
		i::engine_trace->trace_ray(ray, mask_shot_hull, &filter, &out_trace);

		float distance = std::clamp(9500.f * out_trace.fraction, 500.f, 9500.f);
		return distance;
	}

	void change_fog()
	{
		auto fog_array = listener_entity->get_entity(ent_fog);
		if (fog_array.empty())
			return;

		for (const auto& f : fog_array)
		{
			auto entity = f.entity;
			if (!entity)
				continue;

			float start = entity->fog_start();
			float end = entity->fog_end();
			color color_primary = entity->fog_color_primary();

			auto& data = fog_data[f.idx];
			if (!g_cfg.misc.nightmode && data.invalid())
				data.init(start, end, color_primary);

			entity->fog_start() = g_cfg.misc.nightmode ? 0.f : data.start;
			entity->fog_end() = g_cfg.misc.nightmode ? calculate_fog_end() : data.end;

			entity->fog_color_primary() = g_cfg.misc.nightmode
				? g_cfg.misc.world_clr[fog] : data.primary_color;
		}
	}

	void disable_exposure()
	{
		auto tonemap_array = listener_entity->get_entity(ent_tonemap);
		if (tonemap_array.empty())
			return;

		for (const auto& tonemap : tonemap_array)
		{
			auto entity = tonemap.entity;
			if (!entity)
				continue;
			
			// allow to use custom bloom scale and force it to zero
			// because it makes game bright af
			*(bool*)((uintptr_t)entity + offsets::m_bUseCustomBloomScale) = true;
			*(bool*)((uintptr_t)entity + offsets::m_bUseCustomAutoExposureMin) = true;
			*(bool*)((uintptr_t)entity + offsets::m_bUseCustomAutoExposureMax) = true;

			*(float*)((uintptr_t)entity + offsets::m_flCustomBloomScale) = 0.f;
			*(float*)((uintptr_t)entity + offsets::m_flCustomAutoExposureMin) = 0.f;
			*(float*)((uintptr_t)entity + offsets::m_flCustomAutoExposureMax) = 0.f;
		}
	}

	void start(int stage)
	{
		if (!g::local())
			return;

		if (!g::in_game)
			return;

		if (stage != frame_render_start)
			return;

		change_skybox();
		change_fog();

		disable_exposure();
	}
}