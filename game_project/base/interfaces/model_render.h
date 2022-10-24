#pragma once
#include "../ingame objects/c_material.h"
#include "../ingame objects/c_studio_model.h"

#pragma studio_flags
#define studio_none 0x00000000
#define studio_should_render 0x00000001
#define studio_viewxformattachments 0x00000002
#define studio_drawtranslucentsubmodels 0x00000004
#define studio_twopass 0x00000008
#define studio_static_lighting 0x00000010
#define studio_wireframe 0x00000020
#define studio_item_blink 0x00000040
#define studio_noshadows 0x00000080
#define studio_wireframe_vcollide 0x00000100
#define studio_nolighting_or_cubemap 0x00000200
#define studio_skip_flexes 0x00000400
#define studio_donotmodifystencilstate 0x00000800
#define studio_skip_decals 0x10000000
#define studio_shadowtexture 0x20000000
#define studio_shadowdepthtexture 0x40000000
#define studio_transparency 0x80000000

#define render_glow_and_shadow (studio_should_render | studio_skip_flexes | studio_donotmodifystencilstate | studio_nolighting_or_cubemap | studio_skip_decals)
#pragma endregion

class c_renderable;

struct draw_model_state_t
{
	void* studio_hdr;
	void* studio_hdr_data;
	void* renderable;
	const matrix3x4_t* model_to_world;
	void* decals;
	int draw_flags;
	int lod;

	inline draw_model_state_t operator=(const draw_model_state_t& other)
	{
		memcpy(this, &other, sizeof(draw_model_state_t));
		return *this;
	}
};

struct renderable_info
{
	c_renderable* m_renderable;
	void* m_alpha_property;
	int m_enum_count;
	int m_render_frame;
	unsigned short m_first_shadow;
	unsigned short m_leaf_list;
	short m_area;
	uint16_t m_flags;
	uint16_t m_flags2;
	vector3d m_vec_bloated_abs_mins;
	vector3d m_vec_bloated_abs_maxs;
	vector3d m_vec_abs_mins;
	vector3d m_vec_abs_maxs;
	int pad;
};

struct model_render_info_t
{
	vector3d origin;
	vector3d angles;
	char pad[4];
	void* renderable;
	const model_t* model;
	const matrix3x4_t* model_to_world;
	const matrix3x4_t* lightning_offset;
	const vector3d* lightning_origin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	unsigned short instance;

	inline model_render_info_t operator=(const model_render_info_t& other)
	{
		memcpy(this, &other, sizeof(model_render_info_t));
		return *this;
	}
};

class c_model_render
{
public:
	virtual int unk() = 0;
	virtual void forced_material_override(c_material* material, int type = 0, int overrides = 0) = 0;

	void suppress_engine_lighting(bool suppress)
	{
		using fn = void(__thiscall*)(void*, bool);
		memory::vfunc<fn>(this, 24)(this, suppress);
	}
};