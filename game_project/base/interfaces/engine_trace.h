#pragma once
#include "../tools/math.h"

#pragma surf_flags
#define dispsurf_flag_surface           (1<<0)
#define dispsurf_flag_walkable          (1<<1)
#define dispsurf_flag_buildable         (1<<2)
#define dispsurf_flag_surfprop1         (1<<3)
#define dispsurf_flag_surfprop2         (1<<4)
#pragma endregion

#pragma contents_type
#define contents_empty                0

#define contents_solid                0x1       
#define contents_window               0x2
#define contents_aux                  0x4
#define contents_grate                0x8
#define contents_slime                0x10
#define contents_water                0x20
#define contents_blocklos             0x40 
#define contents_opaque               0x80 
#define last_visible_contents         contents_opaque

#define all_visible_contents            (last_visible_contents | (last_visible_contents-1))

#define contents_testfogvolume        0x100
#define contents_unused               0x200     
#define contents_blocklight           0x400
#define contents_team1                0x800 
#define contents_team2                0x1000 
#define contents_ignore_nodraw_opaque 0x2000
#define contents_moveable             0x4000
#define contents_areaportal           0x8000
#define contents_playerclip           0x10000
#define contents_monsterclip          0x20000
#define contents_current_0            0x40000
#define contents_current_90           0x80000
#define contents_current_180          0x100000
#define contents_current_270          0x200000
#define contents_current_up           0x400000
#define contents_current_down         0x800000

#define contents_origin               0x1000000 

#define contents_monster              0x2000000 
#define contents_debris               0x4000000
#define contents_detail               0x8000000 
#define contents_translucent          0x10000000
#define contents_ladder               0x20000000
#define contents_hitbox               0x40000000
#pragma endregion

#pragma region surf_types
#define surf_light                    0x0001 
#define surf_sky2d                    0x0002 
#define surf_sky                      0x0004 
#define surf_warp                     0x0008 
#define surf_trans                    0x0010
#define surf_noportal                 0x0020 
#define surf_trigger                  0x0040 
#define surf_nodraw                   0x0080 

#define surf_hint                     0x0100 

#define surf_skip                     0x0200   
#define surf_nolight                  0x0400   
#define surf_bumplight                0x0800   
#define surf_noshadows                0x1000   
#define surf_nodecals                 0x2000   
#define surf_nopaint                  surf_nodecals
#define surf_nochop                   0x4000   
#define surf_hitbox                   0x8000   
#pragma endregion

#pragma trace_masks
#define mask_all                      (0xffffffff)
#define mask_solid                    (contents_solid|contents_moveable|contents_window|contents_monster|contents_grate)
#define mask_playersolid              (contents_solid|contents_moveable|contents_playerclip|contents_window|contents_monster|contents_grate)
#define mask_npcsolid                 (contents_solid|contents_moveable|contents_monsterclip|contents_window|contents_monster|contents_grate)
#define mask_npcfluid                 (contents_solid|contents_moveable|contents_monsterclip|contents_window|contents_monster)
#define mask_water                    (contents_water|contents_moveable|contents_slime)
#define mask_opaque                   (contents_solid|contents_moveable|contents_opaque)
#define mask_opaque_and_npcs          (mask_opaque|contents_monster)
#define mask_blocklos                 (contents_solid|contents_moveable|contents_blocklos)
#define mask_blocklos_and_npcs        (mask_blocklos|contents_monster)
#define mask_visible                  (mask_opaque|contents_ignore_nodraw_opaque)
#define mask_visible_and_npcs         (mask_opaque_and_npcs|contents_ignore_nodraw_opaque)
#define mask_shot                     (contents_solid|contents_moveable|contents_monster|contents_window|contents_debris|contents_hitbox)
#define mask_shot_brushonly           (contents_solid|contents_moveable|contents_window|contents_debris)
#define mask_shot_hull                (contents_solid|contents_moveable|contents_monster|contents_window|contents_debris|contents_grate)
#define mask_shot_portal              (contents_solid|contents_moveable|contents_window|contents_monster)
#define mask_solid_brushonly          (contents_solid|contents_moveable|contents_window|contents_grate)
#define mask_playersolid_brushonly    (contents_solid|contents_moveable|contents_window|contents_playerclip|contents_grate)
#define mask_npcsolid_brushonly       (contents_solid|contents_moveable|contents_window|contents_monsterclip|contents_grate)
#define mask_npcworldstatic           (contents_solid|contents_window|contents_monsterclip|contents_grate)
#define mask_npcworldstatic_fluid     (contents_solid|contents_window|contents_monsterclip)
#define mask_splitareaportal          (contents_water|contents_slime)
#define mask_current                  (contents_current_0|contents_current_90|contents_current_180|contents_current_270|contents_current_up|contents_current_down)
#define mask_deadsolid                (contents_solid|contents_playerclip|contents_window|contents_grate)
#pragma endregion

using should_hit_fn = bool(__cdecl*)(void*, int);

class c_client_entity;
class c_baseentity;
class c_handle_entity;

enum class trace_type
{
	trace_everything = 0,
	trace_world_only,
	trace_entities_only,
	trace_everything_filter_props,
};

struct plane_t
{
	vector3d normal;
	float dist;
	char type;
	char signbits;
	char pad[2];
};

struct ray_t
{
	vector_aligned start;
	vector_aligned delta;
	vector_aligned start_offset;
	vector_aligned extents;

	matrix3x4_t* world_transform;

	bool is_ray;
	bool is_swept;

	ray_t() : world_transform(nullptr) { }

	ray_t(const vector3d& start, const vector3d& end)
	{
		init(start, end);
	}

	ray_t(const vector3d& start, const vector3d& end, const vector3d& mins, const vector3d& maxs)
	{
		init(start, end, mins, maxs);
	}

	void init(const vector3d& start, const vector3d& end)
	{
		this->delta = end - start;

		this->is_swept = delta.length_sqr() != 0.f;

		this->extents = vector_aligned(0.f, 0.f, 0.f);

		this->world_transform = nullptr;
		this->is_ray = true;

		this->start_offset = vector_aligned(0.f, 0.f, 0.f);
		this->start = start;
	}

	void init(const vector3d& start, const vector3d& end, const vector3d& mins, const vector3d& maxs)
	{
		this->delta = end - start;

		this->world_transform = nullptr;
		this->is_swept = delta.length_sqr() != 0.f;

		this->extents = (maxs - mins) * 0.5f;

		this->is_ray = std::isfinite(extents.length_sqr());

		this->start_offset = (maxs + mins) * 0.5f;

		this->start = start + start_offset;

		this->start_offset *= -1.0f;
	}
};

class c_base_trace
{
public:
	vector3d start;
	vector3d end;
	plane_t plane;

	float fraction;

	int contents;
	unsigned short disp_flags;

	bool all_solid;
	bool start_solid;

	c_base_trace() { }
};

struct surface_t
{
	const char* name;
	short surface_props;
	unsigned short flags;
};

class c_game_trace : public c_base_trace
{
public:
	float fraction_left_solid;
	surface_t surface;
	int hitgroup;
	short physics_bone;
	unsigned short world_surface_index;
	c_baseentity* entity;
	int hitbox;

	inline bool hit() const
	{
		return fraction < 1.0 || all_solid || start_solid;
	}

	c_game_trace() { }
private:
};

class i_trace_filter
{
public:
	virtual bool can_hit_entity(c_baseentity* entity, int mask) = 0;
	virtual trace_type get_trace_type() const = 0;
};

class c_trace_filter : public i_trace_filter
{
public:
	c_trace_filter() : skip_entity(nullptr), ignore(nullptr) {}
	c_trace_filter(c_baseentity* skip) : skip_entity(skip) {}

	bool can_hit_entity(c_baseentity* entity, int mask) override
	{
		return skip_entity != entity;
	}

	trace_type get_trace_type() const override
	{
		return trace_type::trace_everything;
	}

	c_baseentity* skip_entity;
	char* ignore;
};

class c_trace_filter_world : public i_trace_filter
{
public:
	bool can_hit_entity(c_baseentity* entity, int mask) override
	{
		return false;
	}

	trace_type get_trace_type() const override
	{
		return trace_type::trace_world_only;
	};
};

class c_trace_filter_world_and_props_only : public i_trace_filter
{
public:
	bool can_hit_entity(c_baseentity* entity, int mask) override
	{
		return false;
	}
	trace_type get_trace_type() const override
	{
		return trace_type::trace_everything;
	}
};

class c_trace_filter_skip_entities : public i_trace_filter
{
public:
	c_trace_filter_skip_entities() {}
	c_trace_filter_skip_entities(c_baseentity* skip_a, c_baseentity* skip_b)
	{
		this->skip_a = skip_a;
		this->skip_b = skip_b;
	}

	bool can_hit_entity(c_baseentity* entity, int mask) override
	{
		return !(entity == skip_a || entity == skip_b);
	}

	trace_type get_trace_type() const override
	{
		return trace_type::trace_everything;
	}

	c_baseentity* skip_a;
	c_baseentity* skip_b;
	int collision_group;
};

class c_engine_trace
{
public:
	virtual int get_point_contents(const vector3d& abs_position, unsigned int mask = mask_all, void** entity = nullptr) = 0;
	virtual int get_point_contents_world_only(const vector3d& abs_position, unsigned int mask = mask_all) = 0;
	virtual int get_point_contents_collideable(void* collideable, const vector3d& abs_position) = 0;

	virtual void clip_ray_to_entity(const ray_t& ray, unsigned int mask, c_baseentity* entity, c_game_trace* trace) = 0;
	virtual void clip_ray_to_collideable(const ray_t& ray, unsigned int mask, void* collideable, c_game_trace* trace) = 0;

	virtual void trace_ray(const ray_t& ray, unsigned int mask, i_trace_filter* filter, c_game_trace* trace) = 0;
};