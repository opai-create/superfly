#pragma once
#include "../../config_vars.h"
#include "../../../base/hooks/hooks.h"

#include "../../../base/interfaces/studio_render.h"

class c_csplayer;

class c_chams
{
private:
	struct hook_info_t
	{
		draw_model_fn original;

		c_studio_render* ecx;

		void* results;

		draw_model_info_t info;

		matrix3x4_t* bone_to_world;

		float* flex_weights;
		float* flex_delayed_weights;

		vector3d model_origin;

		int flags;

		hook_info_t() : original(nullptr), ecx(nullptr), results(nullptr), info(), bone_to_world(nullptr),
			flex_weights(nullptr), flex_delayed_weights(nullptr), model_origin(), flags(0)
		{ };

		void init(draw_model_fn original, c_studio_render* ecx, void* results, const draw_model_info_t& info,
			matrix3x4_t* bone_to_world, float* flex_weights, float* flex_delayed_weights, const vector3d& model_origin, int flags)
		{
			this->original = original;

			this->ecx = ecx;

			this->results = results;

			this->info = info;

			this->bone_to_world = bone_to_world;

			this->flex_weights = flex_weights;
			this->flex_delayed_weights = flex_delayed_weights;

			this->model_origin = model_origin;

			this->flags = flags;
		}

		void call_original(matrix3x4_t* matrix = nullptr)
		{
			this->original(this->ecx, this->results, this->info, matrix == nullptr ? this->bone_to_world : matrix,
				this->flex_weights, this->flex_delayed_weights, model_origin, flags);
		}
	};

	c_material* soft;
	c_material* shaded;
	c_material* flat;
	c_material* shiny;

	c_material* get_material(int idx);

	inline bool is_valid_materials()
	{
		if (soft == nullptr
			|| shaded == nullptr
			|| flat == nullptr
			|| shiny == nullptr)
			return false;

		return true;
	}

	bool draw_chams(const chams_t& chams, matrix3x4_t* matrix = nullptr);

	void on_enemy(c_csplayer* player, bool& render);

	bool need_to_watch_behind_local();

	void on_local_attachments(bool& render);
	void on_local_player(c_csplayer* player, bool& render);

	hook_info_t hook_info;
public:
	bool should_render();

	void create_materials();

	void init(draw_model_fn original, c_studio_render* ecx, void* results, const draw_model_info_t& info,
		matrix3x4_t* bone_to_world, float* flex_weights, float* flex_delayed_weights, const vector3d& model_origin, int flags);
};