#include "game_functions.h"

#include "../ingame objects/c_csplayer.h"

namespace game_fn
{
	get_name_fn get_name = nullptr;
	init_key_values_fn init_key_values = nullptr;
	load_from_buffer_fn load_from_buffer = nullptr;
	load_named_sky_fn load_named_sky = nullptr;
	md5_pseudo_random_fn md5_pseudo_random = nullptr;
	physics_run_think_fn physics_run_think = nullptr;
	think_fn think = nullptr;
	create_animstate_fn create_animstate = nullptr;
	reset_animstate_fn reset_animstate = nullptr;
	update_animstate_fn update_animstate = nullptr;
	update_all_viewmodel_addons_fn update_all_viewmodel_addons = nullptr;
	get_viewmodel_fn get_viewmodel = nullptr;
	set_abs_angles_fn set_abs_angles = nullptr;
	set_abs_origin_fn set_abs_origin = nullptr;
	get_pose_parameter_fn get_pose_parameter = nullptr;
	update_merge_cache_fn update_merge_cache = nullptr;
	add_dependencies_fn add_dependencies = nullptr;
	attachments_helper_fn attachments_helper = nullptr;
	copy_to_follow_fn copy_to_follow = nullptr;
	copy_from_follow_fn copy_from_follow = nullptr;
	accumulate_pose_fn accumulate_pose = nullptr;
	invalidate_physics_recursive_fn invalidate_physics_recursive = nullptr;
	bone_merge_invalidate_fn bone_merge_invalidate = nullptr;
	bone_merge_init_fn bone_merge_init = nullptr;
	is_breakable_fn is_breakable = nullptr;
	modify_body_yaw_fn modify_body_yaw = nullptr;
	lookup_bone_fn lookup_bone = nullptr;
	write_user_cmd_fn write_user_cmd = nullptr;

	bool* override_processing = nullptr;
	int smoke_count = 0;

	void parse()
	{
		log_console("Parsing in-game function pointers... \n");
		
		get_name						= patterns[pattern_t::get_name].as<get_name_fn>();
		init_key_values					= patterns[pattern_t::init_key_values].as<init_key_values_fn>();
		load_from_buffer				= patterns[pattern_t::load_from_buffer].as<load_from_buffer_fn>();
		load_named_sky					= patterns[pattern_t::load_named_sky].as<load_named_sky_fn>();
		md5_pseudo_random				= patterns[pattern_t::md5_pseudo_random].as<md5_pseudo_random_fn>();
		physics_run_think				= patterns[pattern_t::physics_run_think].as<physics_run_think_fn>();
		think							= patterns[pattern_t::think].as<think_fn>();
		create_animstate				= patterns[pattern_t::create_animstate].as<create_animstate_fn>();
		reset_animstate					= patterns[pattern_t::reset_animstate].as<reset_animstate_fn>();
		update_animstate				= patterns[pattern_t::update_animstate].as<update_animstate_fn>();
		get_viewmodel					= patterns[pattern_t::get_viewmodel].as<get_viewmodel_fn>();
		set_abs_angles					= patterns[pattern_t::set_abs_angles].as<set_abs_angles_fn>();
		set_abs_origin					= patterns[pattern_t::set_abs_origin].as<set_abs_origin_fn>();
		get_pose_parameter				= patterns[pattern_t::get_pose_parameter].as<get_pose_parameter_fn>();
		update_merge_cache				= patterns[pattern_t::update_merge_cache].as<update_merge_cache_fn>();
		add_dependencies				= patterns[pattern_t::add_dependencies].as<add_dependencies_fn>();
		attachments_helper				= patterns[pattern_t::attachments_helper].as<attachments_helper_fn>();
		copy_to_follow					= patterns[pattern_t::copy_to_follow].as<copy_to_follow_fn>();
		copy_from_follow				= patterns[pattern_t::copy_from_follow].as<copy_from_follow_fn>();
		accumulate_pose					= patterns[pattern_t::accumulate_pose].as<accumulate_pose_fn>();
		invalidate_physics_recursive	= patterns[pattern_t::invalidate_physics_recursive].as<invalidate_physics_recursive_fn>();
		bone_merge_invalidate			= patterns[pattern_t::bone_merge_invalidate].as<bone_merge_invalidate_fn>();
		bone_merge_init					= patterns[pattern_t::bone_merge_init].as<bone_merge_init_fn>();
		update_all_viewmodel_addons		= patterns[pattern_t::update_all_viewmodel_addons].as<update_all_viewmodel_addons_fn>();
		is_breakable					= patterns[pattern_t::is_breakable].as<is_breakable_fn>();
		modify_body_yaw					= patterns[pattern_t::modify_body_yaw].as<modify_body_yaw_fn>();
		lookup_bone						= patterns[pattern_t::lookup_bone].as<lookup_bone_fn>();
		write_user_cmd					= patterns[pattern_t::write_user_cmd].as<write_user_cmd_fn>();

		override_processing				= *patterns[pattern_t::disable_post_process].as<bool**>();
		smoke_count						= *patterns[pattern_t::remove_smoke].as<int*>();
		log_console(" \n");
	}
}