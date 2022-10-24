#pragma once
#include "../sdk.h"

#include "../tools/math.h"
#include "../tools/memory.h"

#include "../../base/global_variables.h"
#include "../../base/tools/netvar_parser.h"

#include "c_baseentity.h"
#include "c_animstate.h"
#include "c_animationlayer.h"

#pragma region bone_mask
#define bone_used_mask				0x000FFF00
#define bone_used_by_anything		0x000FFF00
#define bone_used_by_hitbox			0x00000100
#define bone_used_by_attachment		0x00000200
#define bone_used_by_vertex_mask	0x0003FC00
#define bone_used_by_vertex_lod0	0x00000400
#define bone_used_by_vertex_lod1	0x00000800
#define bone_used_by_vertex_lod2	0x00001000
#define bone_used_by_vertex_lod3	0x00002000
#define bone_used_by_vertex_lod4	0x00004000
#define bone_used_by_vertex_lod5	0x00008000
#define bone_used_by_vertex_lod6	0x00010000
#define bone_used_by_vertex_lod7	0x00020000
#define bone_used_by_bone_merge		0x00040000
#define bone_always_setup			0x00080000

#define bone_used_by_vertex_at_lod(lod) ( bone_used_by_vertex_lod0 << (lod) )
#define bone_used_by_anything_at_lod(lod) ( ( bone_used_by_anything & ~bone_used_by_vertex_mask ) | bone_used_by_vertex_at_lod(lod) )

#define setup_mask (bone_used_by_vertex_mask | bone_used_by_attachment | bone_used_by_hitbox)
#pragma endregion

#pragma entity_flags
#define fl_onground (1 << 0)
#define fl_ducking (1 << 1)
#define fl_waterjump (1 << 3)
#define fl_ontrain (1 << 4) 
#define fl_inrain (1 << 5)
#define fl_frozen (1 << 6) 
#define fl_atcontrols (1 << 7) 
#define fl_client (1 << 8)
#define fl_fakeclient (1 << 9)
#define fl_inwater (1 << 10)
#pragma endregion

class c_basecombatweapon;

class c_bone_accessor
{
public:
	alignas(16) matrix3x4_t* bones;

	int readable_bones;
	int writable_bones;
};

struct datamap_t;
class typedescription_t;

struct inputdata_t;
typedef enum _fieldtypes
{
	FIELD_VOID = 0,			// No type or value
	FIELD_FLOAT,			// Any floating point value
	FIELD_STRING,			// A string ID (return from ALLOC_STRING)
	FIELD_VECTOR,			// Any vector, QAngle, or AngularImpulse
	FIELD_QUATERNION,		// A quaternion
	FIELD_INTEGER,			// Any integer or enum
	FIELD_BOOLEAN,			// boolean, implemented as an int, I may use this as a hint for compression
	FIELD_SHORT,			// 2 byte integer
	FIELD_CHARACTER,		// a byte
	FIELD_COLOR32,			// 8-bit per channel r,g,b,a (32bit color)
	FIELD_EMBEDDED,			// an embedded object with a datadesc, recursively traverse and embedded class/structure based on an additional typedescription
	FIELD_CUSTOM,			// special type that contains function pointers to it's read/write/parse functions

	FIELD_CLASSPTR,			// CBaseEntity *
	FIELD_EHANDLE,			// Entity handle
	FIELD_EDICT,			// edict_t *

	FIELD_POSITION_VECTOR,	// A world coordinate (these are fixed up across level transitions automagically)
	FIELD_TIME,				// a floating point time (these are fixed up automatically too!)
	FIELD_TICK,				// an integer tick count( fixed up similarly to time)
	FIELD_MODELNAME,		// Engine string that is a model name (needs precache)
	FIELD_SOUNDNAME,		// Engine string that is a sound name (needs precache)

	FIELD_INPUT,			// a list of inputed data fields (all derived from CMultiInputVar)
	FIELD_FUNCTION,			// A class function pointer (Think, Use, etc)

	FIELD_VMATRIX,			// a vmatrix (output coords are NOT worldspace)

	// NOTE: Use float arrays for local transformations that don't need to be fixed up.
	FIELD_VMATRIX_WORLDSPACE,// A VMatrix that maps some local space to world space (translation is fixed up on level transitions)
	FIELD_MATRIX3X4_WORLDSPACE,	// matrix3x4_t that maps some local space to world space (translation is fixed up on level transitions)

	FIELD_INTERVAL,			// a start and range floating point interval ( e.g., 3.2->3.6 == 3.2 and 0.4 )
	FIELD_MODELINDEX,		// a model index
	FIELD_MATERIALINDEX,	// a material index (using the material precache string table)

	FIELD_VECTOR2D,			// 2 floats

	FIELD_TYPECOUNT,		// MUST BE LAST
} fieldtype_t;

enum
{
	td_offset_normal = 0,
	td_offset_packed = 1,
	td_offset_count,
};

class typedescription_t
{
public:
	int fieldType;
	const char* fieldName;
	int fieldOffset;
	unsigned short fieldSize;
	short flags;

	const char* externalName;

	void* pSaveRestoreOps;

	void* inputFunc;

	datamap_t* td;

	int fieldSizeInBytes;

	class typedescription_t* override_field;

	int override_count;

	float fieldTolerance;

	int flatOffset[td_offset_count];
	unsigned short flatGroup;
};

struct datamap_t
{
	typedescription_t* dataDesc;
	int dataNumFields;
	const char* dataClassName;
	datamap_t* baseMap;
	int m_nPackedSize;
	void* m_pOptimizedDataMap;
};

unsigned int find_in_datamap(datamap_t* pMap, const char* name);

class c_csplayer : public c_baseentity
{
public:
	ADD_VAR(velocity, vector3d, this, offsets::m_vecVelocity)
	ADD_VAR_REF(aim_punch_angle_vel, vector3d, this, offsets::m_aimPunchAngleVel)
	ADD_VAR_REF(view_punch_angle, vector3d, this, offsets::m_viewPunchAngle)
	ADD_VAR_REF(aim_punch_angle, vector3d, this, offsets::m_aimPunchAngle)
	ADD_VAR(hitbox_set, int, this, offsets::m_nHitboxSet)
	ADD_VAR(flash_time, int, this, offsets::m_flFlashtime)
	ADD_VAR(sequence, int, this, offsets::m_nSequence)
	ADD_VAR(health, int, this, offsets::m_iHealth)
	ADD_VAR(team, int, this, offsets::m_iTeamNum)
	ADD_VAR(money, int, this, offsets::m_iAccount)
	ADD_VAR_REF(default_fov, int, this, offsets::m_iDefaultFOV)
	ADD_VAR_REF(fov_start, int, this, offsets::m_iFOVStart)
	ADD_VAR_REF(fov, int, this, offsets::m_iFOV)
	ADD_VAR_REF(tick_base, int, this, offsets::m_nTickBase)
	ADD_VAR_REF(flags, int, this, offsets::m_fFlags)
	ADD_VAR_REF(effects, int, this, 0xF0)
	ADD_VAR_REF(e_flags, int, this, 0xE8)
	ADD_VAR_REF(ik_context_ptr, int, this, 0x99F)
	ADD_VAR_REF(obs_server_mode, int, this, offsets::m_iObserverMode)
	ADD_VAR_REF(ent_flags, int, this, 0x68)
	ADD_VAR_REF(armor, int, this, offsets::m_ArmorValue)
	ADD_VAR_REF(anim_flags, int, this, 0xA28)
	ADD_VAR(surface_friction, float, this, 0x323C)
	ADD_VAR(next_attack, float, this, offsets::m_flNextAttack)
	ADD_VAR(spawn_time, float, this, offsets::m_iAddonBits - 4)
	ADD_VAR(cycle, float, this, offsets::m_flCycle)
	ADD_VAR_REF(last_setup_bone_time, float, this, 0x2924)
	ADD_VAR_REF(thirdperson_recoil, float, this, offsets::m_flThirdpersonRecoil)
	ADD_VAR_REF(old_simulation_time, float, this, offsets::m_flSimulationTime + 4)
	ADD_VAR_REF(flash_max_alpha, float, this, offsets::m_flFlashMaxAlpha)
	ADD_VAR_REF(simulation_time, float, this, offsets::m_flSimulationTime)
	ADD_VAR_REF(duck_amount, float, this, offsets::m_flDuckAmount)
	ADD_VAR_REF(lby, float, this, offsets::m_flLowerBodyYawTarget)
	ADD_VAR(have_defuser, bool, this, offsets::m_bHasDefuser)
	ADD_VAR(have_immunity, bool, this, offsets::m_bGunGameImmunity)
	ADD_VAR(is_defusing, bool, this, offsets::m_bIsDefusing)
	ADD_VAR(has_helmet, bool, this, offsets::m_bHasHelmet)
	ADD_VAR(is_scoped, bool, this, offsets::m_bIsScoped)
	ADD_VAR(has_heavy_armor, bool, this, offsets::m_bHasHeavyArmor)
	ADD_VAR_REF(clientside_anim, bool, this, offsets::m_bClientSideAnimation)
	ADD_VAR_REF(jiggle_bones_enabled, bool, this, 0x2930)
	ADD_VAR_REF(is_walking, bool, this, offsets::m_bIsWalking)
	ADD_VAR_PTR2(get_animation_state, c_animstate, this, 0x9960)
	ADD_VAR_PTR2(get_animation_layer, c_animationlayer, this, 0x2990)
	ADD_VAR_PTR(get_bone_accessor, c_bone_accessor, this, offsets::m_nForceBone + 28)
	ADD_VAR_REF(get_bone_cache, c_utlvector<matrix3x4_t>, this, 0x2914)
	ADD_VAR_REF(model_recent_bone_counter, uint32_t, this, 0x2690)

	bool is_alive() { return health() > 0; }

	inline void parse_layer(c_animationlayer* layer)
	{
		std::memcpy(layer, this->get_animation_layer(), sizeof(c_animationlayer) * 13);
	}

	inline void set_layer(c_animationlayer* layer)
	{
		std::memcpy(this->get_animation_layer(), layer, sizeof(c_animationlayer) * 13);
	}

	inline void parse_pose(std::array<float, 24>& pose)
	{
		std::memcpy(pose.data(), this->pose_parameter().data(), sizeof(float) * 24);
	}

	inline void set_pose(std::array<float, 24> pose)
	{
		std::memcpy(this->pose_parameter().data(), pose.data(),  sizeof(float) * 24);
	}

	// 8 is an EFL_NOINTERP flag
	void disable_interpolation()
	{
		this->effects() |= 8;
	}

	void enable_interpolation()
	{
		this->effects() &= ~8;
	}

	void remove_game_kinematic()
	{
		this->ent_flags() |= 2; // ENTCLIENTFLAG_DONTUSEIK
		this->ik_context_ptr() = 0;
		this->anim_flags() &= ~0xA; // ANIMLODFLAG_DORMANT | ANIMLODFLAG_OUTSIDEVIEWFRUSTUM 
	}

	void invalidate_bone_cache()
	{
		uintptr_t cache = *patterns[pattern_t::invalidate_bone_cache].add(0xA).as<uintptr_t*>();

		this->model_recent_bone_counter() = (*(uint32_t*)cache) - 1;
		this->get_bone_accessor()->readable_bones 
			= this->get_bone_accessor()->writable_bones = -1;
		this->last_setup_bone_time() = -FLT_MAX;
	}

	bool is_bot()
	{
		player_info_t player_info = {};
		i::engine->get_player_info(this->index(), &player_info);
		return player_info.fake_player;
	}

	void update_clientside_animation()
	{
		using fn = void(__thiscall*)(void*);
		memory::vfunc<fn>(this, 224)(this);
	}

	void update_dispatch_layer(c_animationlayer* layers, c_studiohdr* hdr, int sequence)
	{
		using fn = void(__thiscall*)(void*, c_animationlayer*, c_studiohdr*, int);
		memory::vfunc<fn>(this, 247)(this, layers, hdr, sequence);
	}
	
	void calc_ik_locks(float curtime)
	{
		using fn = void(__thiscall*)(void*, float);
		memory::vfunc<fn>(this, 192)(this, curtime);
	}

	datamap_t* get_pred_desc_map()
	{
		using fn = datamap_t*(__thiscall*)(void*);
		return memory::vfunc<fn>(this, 17)(this);
	}

	vector3d bone_position(int bone_index = 8);
	vector3d hitbox_position(int hitbox = 0, matrix3x4_t* matrix = nullptr);
	c_basecombatweapon* get_active_weapon();
	c_studiohdr* get_studio_hdr();

	void run_pre_think();
	void run_think();

	vector3d eye_pos();
};