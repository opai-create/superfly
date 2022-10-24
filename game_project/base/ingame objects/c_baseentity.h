#pragma once
#include "../sdk.h"

#include "../tools/math.h"
#include "../tools/memory.h"
#include "../tools/netvar_parser.h"

#include "../other/game_functions.h"

#include "../global_variables.h"

class c_baseentity;
class c_csplayer;
class c_basecombatweapon;

class c_renderable;
class c_networkable;
class c_thinkable;
class c_unknown;
class c_client_entity;

enum move_type
{
	movetype_none = 0,			// never moves
	movetype_isometric,			// for players -- in tf2 commander view, etc.
	movetype_walk,				// player only - moving on the ground
	movetype_step,				// gravity, special edge handling -- monsters use this
	movetype_fly,				// no gravity, but still collides with stuff
	movetype_flygravity,		// flies through the air + is affected by gravity
	movetype_vphysics,			// uses vphysics for simulation
	movetype_push,				// no clip to world, push and crush
	movetype_noclip,			// no gravity, no collisions, still do velocity/avelocity
	movetype_ladder,			// used by players only when going onto a ladder
	movetype_observer,			// observer movement, depends on player's observer mode
	movetype_custom,			// allows the entity to describe its own physics

	// should always be defined as the last item in the list
	movetype_last = movetype_custom,

	movetype_max_bits = 4
};

class c_ikcontext
{
public:
	void construct()
	{
		using IKConstruct = void(__thiscall*)(void*);
		static auto ik_ctor = patterns[ik_ctx_construct].as<IKConstruct>();

		ik_ctor(this);
	}

	void destruct()
	{
		using IKDestructor = void(__thiscall*)(c_ikcontext*);
		static auto ik_dector = patterns[ik_ctx_destruct].as<IKDestructor>();

		ik_dector(this);
	}

	void clear_targets()
	{
		auto i = 0;
		auto count = *reinterpret_cast <int*> ((uintptr_t)this + 0xFF0);

		if (count > 0)
		{
			auto target = reinterpret_cast <int*> ((uintptr_t)this + 0xD0);

			do
			{
				*target = -9999;
				target += 85;
				++i;
			} while (i < count);
		}
	}

	void init(c_studiohdr* hdr, vector3d* angles, vector3d* origin, float time, int frames, int mask)
	{
		using init_fn = void(__thiscall*)(void*, c_studiohdr*, vector3d*, vector3d*, float, int, int);
		patterns[ik_ctx_init].as<init_fn>()(this, hdr, angles, origin, time, frames, mask);
	}

	void update_targets(vector3d* pos, quaternion* qua, matrix3x4_t* matrix, uint8_t* boneComputed)
	{
		using update_targets_fn = void(__thiscall*)(void*, vector3d*, quaternion*, matrix3x4_t*, uint8_t*);
		patterns[ik_ctx_update_targets].as<update_targets_fn>()(this, pos, qua, matrix, boneComputed);
	}

	void solve_dependencies(vector3d* pos, quaternion* qua, matrix3x4_t* matrix, uint8_t* boneComputed)
	{
		using solve_dependencies_fn = void(__thiscall*)(void*, vector3d*, quaternion*, matrix3x4_t*, uint8_t*);
		patterns[ik_ctx_solve_dependencies].as<solve_dependencies_fn>()(this, pos, qua, matrix, boneComputed);
	}
};

struct bone_setup_t
{
	c_studiohdr* hdr;
	int mask;
	float* pose_parameter;
	void* pose_debugger;

	void init_pose(vector3d pos[], quaternion q[], c_studiohdr* hdr)
	{
		static auto init_pose = patterns[bone_setup_init_pose].as<uint64_t>();

		__asm
		{
			mov eax, this
			mov esi, q
			mov edx, pos
			push dword ptr[hdr + 4]
			mov ecx, [eax]
			push esi
			call init_pose
			add esp, 8
		}
	}

	void accumulate_pose(vector3d pos[], quaternion q[], int sequence, float cycle, float weight, float time, c_ikcontext* ctx)
	{
		game_fn::accumulate_pose(this, pos, q, sequence, cycle, weight, time, ctx);
	}	 

	void calc_autoplay_sequences(vector3d pos[], quaternion q[], float real_time, c_ikcontext* ctx)
	{
		static auto calc_autoplay_sequences = patterns[bone_setup_calc_autoplay_sequences].as<uint64_t>();

		__asm
		{
			movss xmm3, real_time
			mov eax, ctx
			mov ecx, this
			push eax
			push q
			push pos
			call calc_autoplay_sequences
		}
	}

	void calc_bone_adjust(vector3d pos[], quaternion q[], float* controllers, int mask)
	{
		static auto calc_bone_adj = patterns[bone_setup_calc_bone_adjust].as<uint64_t>();

		__asm
		{
			mov     eax, controllers
			mov     ecx, this
			mov     edx, pos; a2
			push    dword ptr[ecx + 4]; a5
			mov     ecx, [ecx]; a1
			push    eax; a4
			push    q; a3
			call    calc_bone_adj
			add     esp, 0xC
		}
	}
};

class c_handle_entity 
{
public:
	virtual ~c_handle_entity() {}
	virtual void set_ref_handle(const uint32_t& handle) = 0;
	virtual const uint32_t& get_ref_handle() const = 0;
};

class c_collideable
{
public:
	virtual c_handle_entity* get_entity_handle() = 0;
	virtual vector3d& get_mins() const = 0;
	virtual vector3d& get_maxs() const = 0;
};

class c_unknown : public c_handle_entity
{
public:
	virtual c_collideable* get_collideable() = 0;
	virtual c_networkable* get_networkable_entity() = 0;
	virtual c_renderable* get_renderable_entity() = 0;
	virtual c_client_entity* get_client_entity() = 0;
	virtual c_baseentity* get_base_entity() = 0;
	virtual c_thinkable* get_thinkable_entity() = 0;
	virtual void* get_client_alpha_property() = 0;
};

class c_renderable
{
public:
	virtual c_unknown* get_i_unknown_entity() = 0;

	virtual const vector3d& get_render_origin(void) = 0;
	virtual const vector3d& get_render_angles(void) = 0;

	virtual bool should_render(void) = 0;
	virtual int get_render_flags(void) = 0;

	virtual void unused(void) const {}

	virtual void* get_shadow_handle() const = 0;
	virtual void*& get_render_handle() = 0;

	virtual model_t* get_model() const = 0;
	virtual int draw_model(int flags, const int& instance) = 0;

	virtual int get_body() = 0;

	virtual void get_color_modulation(float* color) = 0;

	virtual bool lod_test() = 0;

	virtual bool setup_bones(matrix3x4_t* matrix, int max_bones, int mask, float time) = 0;
	virtual void setup_weights(const matrix3x4_t* matrix, int flex_cnt, float* flex_weights, float* flex_delayed_weights) = 0;

	virtual void useless_func1(void) = 0;

	virtual void* useless_func2() = 0;

	virtual void get_render_bounds(vector3d& mins, vector3d& maxs) = 0;
};

class c_networkable
{
public:
	virtual c_unknown* get_n_unknown_entity() = 0;
	virtual void release() = 0;
	virtual c_client_class* get_client_class() = 0;
	virtual void should_transmit(int state) = 0;
	virtual void pre_data_changed(int update_type) = 0;
	virtual void data_changed(int update_type) = 0;
	virtual void pre_update(int update_type) = 0;
	virtual void pust_update(int update_type) = 0;
	virtual void unk(void) = 0;
	virtual bool dormant(void) = 0;
	virtual int index(void) = 0;
};

class c_thinkable
{
public:
	virtual c_unknown* get_t_unknown_entity() = 0;
};

class c_client_entity : public c_unknown, public c_renderable, public c_networkable, public c_thinkable
{
public:
	virtual void release(void) = 0;

	virtual const vector3d& get_abs_origin(void) = 0;
	virtual const vector3d& get_abs_angles(void) = 0;
};

class c_baseentity : public c_client_entity
{
public:
	ADD_VAR(view_offset, vector3d, this, offsets::m_vecViewOffset)
	ADD_VAR(angle_rotarion, vector3d, this, offsets::m_angAbsRotation)
	ADD_VAR_REF(render_eye_angles, vector3d, this, offsets::deadflag + 4)
	ADD_VAR_REF(eye_angles, vector3d, this, offsets::m_angEyeAngles)
	ADD_VAR_REF(origin, vector3d, this, offsets::m_vecOrigin)
	ADD_VAR_REF(mins, vector3d, this, offsets::m_vecMins)
	ADD_VAR_REF(maxs, vector3d, this, offsets::m_vecMaxs)
	ADD_VAR(smoke_effect_tick_begin, int, this, offsets::m_nSmokeEffectTickBegin)
	ADD_VAR(owner_handle, int, this, offsets::m_hOwnerEntity)
	ADD_VAR(fire_count, int, this, offsets::m_fireCount)
	ADD_VAR(move_type, int, this, offsets::m_MoveType)
	ADD_VAR_REF(get_skybox_area, int, this, offsets::m_skyBoxArea)
	ADD_VAR_PTR(fire_x_delta, int, this, offsets::m_fireXDelta)
	ADD_VAR_PTR(fire_y_delta, int, this, offsets::m_fireYDelta)
	ADD_VAR_PTR(fire_z_delta, int, this, offsets::m_fireZDelta)
	ADD_VAR(bomb_defuse_cooldown, float, this, offsets::m_flDefuseCountDown)
	ADD_VAR(bomb_timer_length, float, this, offsets::m_flTimerLength)
	ADD_VAR(bomb_blow, float, this, offsets::m_flC4Blow)
	ADD_VAR_REF(velocity_modifier, float, this, offsets::m_flVelocityModifier)
	ADD_VAR_REF(fog_start, float, this, offsets::m_fogStart)
	ADD_VAR_REF(fog_end, float, this, offsets::m_fogEnd)
	ADD_VAR(did_smoke_effect, bool, this, offsets::m_bDidSmokeEffect)
	ADD_VAR(is_bomb_ticking, bool, this, offsets::m_bBombTicking)
	ADD_VAR(is_bomb_defused, bool, this, offsets::m_bBombDefused)
	ADD_VAR(nade_exploded, bool, this, offsets::m_nExplodeEffectTickBegin + 4)
	ADD_VAR_PTR(fire_burning, bool, this, offsets::m_bFireIsBurning)
	ADD_VAR_REF(fog_color_primary, color, this, offsets::m_fogColorPrimary)
	ADD_VAR(coordinate_frame, matrix3x4_t, this, offsets::m_CollisionGroup - 0x30)
	ADD_VAR(active_weapon, uint32_t, this, offsets::m_hActiveWeapon)
	ADD_VAR(bomb_defuser, uint32_t, this, offsets::m_hBombDefuser)
	ADD_VAR(move_parent, uint32_t, this, offsets::m_hMoveParent)
	ADD_VAR_REF(ground_entity, uint32_t, this, offsets::m_hGroundEntity)

	std::array<float, 24>& pose_parameter() 
	{ 
		return *(std::array<float, 24>*)((uintptr_t)this + offsets::m_flPoseParameter);
	}

	bool is_player()
	{
		using fn = bool(__thiscall*)(void*);
		return memory::vfunc<fn>(this, 158)(this);
	}

	bool is_weapon()
	{
		using fn = bool(__thiscall*)(void*);
		return memory::vfunc<fn>(this, 166)(this);
	}

	c_baseentity* get_move_parent();
};