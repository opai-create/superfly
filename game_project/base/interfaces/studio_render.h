#pragma once
#include "../tools/math.h"
#include "../tools/memory.h"

#include "app_system.h"

#include "../../base/ingame objects/c_material.h"

struct draw_model_info_t
{
	void* studio_hdr;
	void* hardware_data;
	void* decals;
	int skin;
	int body;
	int hitbox_set;
	void* client_entity;
	int lod;
	void* color_meshes;
	bool static_lightning;
	void* lightning_state;

	inline draw_model_info_t operator=(const draw_model_info_t& other)
	{
		std::memcpy(this, &other, sizeof(draw_model_info_t));
		return *this;
	}
};

class c_studio_render : public c_app_system
{
public:
	void set_color_modulation(float* color)
	{
		using fn = void(__thiscall*)(void*, float*);
		memory::vfunc<fn>(this, 27)(this, color);
	}

	void set_alpha_modulation(float alpha)
	{
		using fn = void(__thiscall*)(void*, float);
		memory::vfunc<fn>(this, 28)(this, alpha);
	}

	void draw_model(void* results, const draw_model_info_t& info,
		matrix3x4_t* bone_to_world, float* flex_weights, float* flex_delayed_weights, const vector3d& model_origin, int flags = 0)
	{
		using fn = void(__thiscall*)(void*, void*, const draw_model_info_t&, matrix3x4_t*, float*, float*, const vector3d, int);
		memory::vfunc<fn>(this, 29)(this, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
	}

	void forced_material_override(c_material* material, int override_type = 0, int material_idx = -1)
	{
		using fn = void(__thiscall*)(void*, c_material*, int, int);
		memory::vfunc<fn>(this, 33)(this, material, override_type, material_idx);
	}

	bool is_forced_material_override()
	{
		using fn = bool(__thiscall*)(void*);
		return memory::vfunc<fn>(this, 34)(this);
	}
};