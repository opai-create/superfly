#pragma once
#include "../tools/math.h"
#include "../tools/memory.h"

class c_animstate;
class c_studiohdr;
class c_ikcontext;
class c_csplayer;
class c_baseentity;
class c_usercmd;

struct mstudio_pose_param_desc_t;
struct bone_setup_t;

namespace game_fn
{
	using get_name_fn = const char* (__thiscall*)(void*);
	extern get_name_fn get_name;

	using init_key_values_fn = void(__thiscall*)(void*, const char*, int, int);
	extern init_key_values_fn init_key_values;

	using load_from_buffer_fn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*, void*);
	extern load_from_buffer_fn load_from_buffer;

	using load_named_sky_fn = void(__fastcall*)(const char*);
	extern load_named_sky_fn load_named_sky;

	using md5_pseudo_random_fn = uint32_t(__thiscall*)(uint32_t);
	extern md5_pseudo_random_fn md5_pseudo_random;

	using physics_run_think_fn = bool(__thiscall*)(void*, int);
	extern physics_run_think_fn physics_run_think;

	using think_fn = void(__thiscall*)(void*, int);
	extern think_fn think;

	using create_animstate_fn = void(__thiscall*)(c_animstate*);
	extern create_animstate_fn create_animstate;

	using reset_animstate_fn = void(__thiscall*)(c_animstate*);
	extern reset_animstate_fn reset_animstate;

	using update_animstate_fn = void(__vectorcall*)(c_animstate*, void*, float, float, float, bool);
	extern update_animstate_fn update_animstate;

	using update_all_viewmodel_addons_fn = int(__fastcall*)(void*);
	extern update_all_viewmodel_addons_fn update_all_viewmodel_addons;

	using get_viewmodel_fn = void* (__thiscall*)(void*, int);
	extern get_viewmodel_fn get_viewmodel;

	using set_abs_angles_fn = void(__thiscall*)(void*, const vector3d&);
	extern set_abs_angles_fn set_abs_angles;

	using set_abs_origin_fn = void(__thiscall*)(void*, const vector3d&);
	extern set_abs_origin_fn set_abs_origin;

	using get_pose_parameter_fn = mstudio_pose_param_desc_t * (__thiscall*)(c_studiohdr*, int);
	extern get_pose_parameter_fn get_pose_parameter;

	using update_merge_cache_fn = void(__thiscall*)(uintptr_t);
	extern update_merge_cache_fn update_merge_cache;

	using add_dependencies_fn = void(__thiscall*)(c_ikcontext*, float, int, int, const float[], float);
	extern add_dependencies_fn add_dependencies;

	using attachments_helper_fn = void(__thiscall*)(c_csplayer*, c_studiohdr*);
	extern attachments_helper_fn attachments_helper;

	using copy_to_follow_fn = void(__thiscall*)(uintptr_t, vector3d*, quaternion*, int, vector3d*, quaternion*);
	extern copy_to_follow_fn copy_to_follow;

	using copy_from_follow_fn = void(__thiscall*)(uintptr_t, vector3d*, quaternion*, int, vector3d*, quaternion*);
	extern copy_from_follow_fn copy_from_follow;

	using accumulate_pose_fn = void(__thiscall*)(bone_setup_t*, vector3d*, quaternion*, int, float, float, float, c_ikcontext*);
	extern accumulate_pose_fn accumulate_pose;

	using invalidate_physics_recursive_fn = void(__thiscall*)(void*, int);
	extern invalidate_physics_recursive_fn invalidate_physics_recursive;

	using bone_merge_invalidate_fn = void(__thiscall*)(uintptr_t);
	extern bone_merge_invalidate_fn bone_merge_invalidate;

	using bone_merge_init_fn = void(__thiscall*)(uintptr_t, void*);
	extern bone_merge_init_fn bone_merge_init;

	using is_breakable_fn = bool(__thiscall*)(c_baseentity*);
	extern is_breakable_fn is_breakable;

	using modify_body_yaw_fn = void(__thiscall*)(c_csplayer*, matrix3x4_t*, int);
	extern modify_body_yaw_fn modify_body_yaw;

	using lookup_bone_fn = int(__thiscall*)(void*, const char*);
	extern lookup_bone_fn lookup_bone;

	using write_user_cmd_fn = void(__fastcall*)(void*, c_usercmd*, c_usercmd*);
	extern write_user_cmd_fn write_user_cmd;

	extern bool* override_processing;
	extern int smoke_count;

	void parse();
}