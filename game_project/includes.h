#pragma once
#include "windows_includes.h"
#include "additionals/memory_related/pattern.h"

enum pattern_t : int
{
	hud_scope_paint = 0,
	get_name,
	init_key_values,
	load_from_buffer,
	list_leaves_in_box,
	get_color_modulation,
	draw_models,
	using_static_prop_debug,
	load_named_sky,
	disable_post_process,
	return_addr_drift_pitch,
	return_addr_apply_shake,
	remove_smoke,
	remove_fog,
	check_file_crc_with_server,
	set_view_model_offsets,
	prediction_random_seed,
	prediction_player,
	md5_pseudo_random,
	physics_run_think,
	think,
	move_helper,
	return_addr_compute_translucency,
	is_thirdperson_overview,
	return_addr_draw_brushes,
	beam,
	add_view_model_bob,
	calc_view_model_bob,
	input,
	player_resource,
	inventory_access,
	return_addr_animstate_pitch,
	return_addr_animstate_yaw,
	get_eye_angles,
	create_animstate,
	reset_animstate,
	update_animstate,
	update_all_viewmodel_addons,
	get_viewmodel,
	client_state,
	set_abs_angles,
	set_abs_origin,
	should_skip_anim_frame,
	modify_eye_position,
	setup_bones,
	bone_cache,
	update_clientside_animation,
	build_transformations,
	standard_blending_rules,
	do_extra_bone_processing,
	retn_to_setup_velocity,
	retn_to_accumulate_layers,
	retn_to_reevaluate_anim_lod,
	retn_to_extrapolation,
	retn_to_setup_bones,
	get_bone_merge,
	get_pose_parameter,
	update_merge_cache,
	ik_context_ptr,
	invalidate_bone_cache,
	custom_player_ptr,
	copy_to_follow,
	copy_from_follow,
	add_dependencies,
	attachments_helper,
	ik_ctx_construct,
	ik_ctx_destruct,
	ik_ctx_init,
	ik_ctx_update_targets,
	ik_ctx_solve_dependencies,
	bone_setup_init_pose,
	accumulate_pose,
	bone_setup_calc_autoplay_sequences,
	bone_setup_calc_bone_adjust,
	studio_hdr_ptr,
	accumulate_pose_debug_break,
	invalidate_physics_recursive,
	bone_merge_invalidate,
	bone_merge_init,
	is_breakable,
	trace_filter,
	trace_filter_skip_entities,
	clip_ray_to_hitboxes,
	modify_body_yaw,
	lookup_bone,
	physics_simulate,
	write_user_cmd,
	pattern_max
};

extern std::array<Address, pattern_max> patterns;

__inline void log_console(const char* msg, ...)
{
#ifdef _DEBUG
	va_list va_alist;
	char buffer[1024] = { '\0' };

	va_start(va_alist, msg);
	vsprintf_s(buffer, msg, va_alist);
	va_end(va_alist);

	printf(buffer);
#endif
}