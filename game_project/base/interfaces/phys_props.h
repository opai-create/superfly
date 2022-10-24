#pragma once
#include "../tools/memory.h"

enum {
	char_tex_antlion = 'a',
	char_tex_bloodyflesh = 'b',
	char_tex_concrete = 'c',
	char_tex_dirt = 'd',
	char_tex_eggshell = 'e',
	char_tex_flesh = 'f',
	char_tex_grate = 'g',
	char_tex_alienflesh = 'h',
	char_tex_clip = 'i',
	char_tex_plastic = 'l',
	char_tex_metal = 'm',
	char_tex_sand = 'n',
	char_tex_foliage = 'o',
	char_tex_computer = 'p',
	char_tex_slosh = 's',
	char_tex_tile = 't',
	char_tex_cardboard = 'u',
	char_tex_vent = 'v',
	char_tex_wood = 'w',
	char_tex_glass = 'y',
	char_tex_warpshield = 'z',
};

struct surfacephysicsparams_t 
{
	float friction;
	float elasticity;
	float density;
	float thickness;
	float dampening;
};

struct surfaceaudioparams_t 
{
	float audio_reflectivity;
	float audio_hardness_factor;
	float audio_roughness_factor;
	float scrape_rough_threshold;
	float impact_hard_threshold;
	float audio_hard_min_velocity;
	float high_pitch_occlusion;
	float mid_pitch_occlusion;
	float low_pitch_occlusion;
};

struct surfacegameprops_t 
{
	float max_speed_factor;
	float jump_factor;
	float penetration_modifier;
	float damage_modifier;
	uint16_t material;
	uint8_t climbable;
};

struct surfacesoundnames_t 
{
	short walk_left;
	short walk_right;
	short run_left;
	short run_right;
	short impact_soft;
	short impact_hard;
	short scrape_smooth;
	short scrape_rough;
	short bullet_impact;
	short rolling;
	short break_sound;
	short strain;
};

struct surfacedata_t 
{
	surfacephysicsparams_t physics;
	surfaceaudioparams_t audio;
	surfacesoundnames_t sounds;
	surfacegameprops_t game;
	char pad[48];
}; 

class c_phys_surface_props 
{
public:
	surfacedata_t* get_surface_data(int surface_index) 
	{
		using fn = surfacedata_t*(__thiscall*)(void*, int);
		return memory::vfunc<fn>(this, 5)(this, surface_index);
	}
};