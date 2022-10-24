#pragma once

class vector3d;

// credits to: l3d451r7

enum animstate_pose_param_idx_t
{
	player_pose_param_first = 0,
	player_pose_param_lean_yaw = player_pose_param_first,
	player_pose_param_speed,
	player_pose_param_ladder_speed,
	player_pose_param_ladder_yaw,
	player_pose_param_move_yaw,
	player_pose_param_run,
	player_pose_param_body_yaw,
	player_pose_param_body_pitch,
	player_pose_param_death_yaw,
	player_pose_param_stand,
	player_pose_param_jump_fall,
	player_pose_param_aim_blend_stand_idle,
	player_pose_param_aim_blend_crouch_idle,
	player_pose_param_strafe_dir,
	player_pose_param_aim_blend_stand_walk,
	player_pose_param_aim_blend_stand_run,
	player_pose_param_aim_blend_crouch_walk,
	player_pose_param_move_blend_walk,
	player_pose_param_move_blend_run,
	player_pose_param_move_blend_crouch_walk,
	player_pose_param_count,
};

struct animstate_pose_param_cache_t
{
	bool initialized;
	int index;
	const char* name;

	animstate_pose_param_cache_t()
	{
		initialized = false;
		index = -1;
		name = "";
	}
};

class c_animstate
{
public:
	int* layer_order_preset = nullptr;

	bool first_run_since_init = false;
		 
	bool first_foot_plant_since_init = false;

	int last_update_tick = 0;

	float eye_position_smooth_lerp = 0.f;
		  
	float strafe_change_weight_smooth_fall_off = 0.f;

	float stand_walk_duration_state_has_been_valid = 0.f;
	float stand_walk_duration_state_has_been_invalid = 0.f;
	float stand_walk_how_long_to_wait_until_transition_can_blend_in = 0.f;
	float stand_walk_how_long_to_wait_until_transition_can_blend_out = 0.f;
	float stand_walk_blend_value = 0.f;
		  
	float stand_run_duration_state_has_been_valid = 0.f;
	float stand_run_duration_state_has_been_invalid = 0.f;
	float stand_run_how_long_to_wait_until_transition_can_blend_in = 0.f;
	float stand_run_how_long_to_wait_until_transition_can_blend_out = 0.f;
	float stand_run_blend_value = 0.f;
		  
	float crouch_walk_duration_state_has_been_valid = 0.f;
	float crouch_walk_duration_state_has_been_invalid = 0.f;
	float crouch_walk_how_long_to_wait_until_transition_can_blend_in = 0.f;
	float crouch_walk_how_long_to_wait_until_transition_can_blend_out = 0.f;
	float crouch_walk_blend_value = 0.f;

	int cached_model_index = 0;

	float step_height_left = 0.f;
	float step_height_right = 0.f;

	void* weapon_last_bone_setup = nullptr;

	void* player = nullptr;
	void* weapon = nullptr;
	void* weapon_last = nullptr;

	float last_update_time = 0.f;

	int last_update_frame = 0;

	float last_update_increment = 0.f;

	float eye_yaw = 0.f; 
	float eye_pitch = 0.f;
	float abs_yaw = 0.f;
	float abs_yaw_last = 0.f;
	float move_yaw = 0.f;
	float move_yaw_ideal = 0.f; 
	float move_yaw_current_to_ideal = 0.f; 
	float time_to_align_lower_body;

	float primary_cycle = 0.f;
	float move_weight = 0.f; 
		  
	float move_weight_smoothed = 0.f;
	float anim_duck_amount = 0.f;
	float duck_additional = 0.f;
	float recrouch_weight = 0.f;

	vector3d position_current = vector3d();
	vector3d position_last = vector3d();
		   	 
	vector3d velocity = vector3d();
	vector3d velocity_normalized = vector3d();
	vector3d velocity_normalized_non_zero = vector3d();

	float velocity_length_xy = 0.f;
	float velocity_length_z = 0.f;

	float speed_as_portion_of_run_top_speed = 0.f;
	float speed_as_portion_of_walk_top_speed = 0.f;
	float speed_as_portion_of_crouch_top_speed = 0.f;

	float duration_moving = 0.f;
	float duration_still = 0.f;

	bool on_ground = false;
	bool landing = false; 

	float jump_to_fall = 0.f;
	float duration_in_air = 0.f; 
	float left_ground_height = 0.f; 
	float land_anim_multiplier = 0.f;

	float walk_run_transition = 0.f;

	bool landed_on_ground_this_frame = false;
	bool left_the_ground_this_frame = false;
	float in_air_smooth_value = 0.f;

	bool on_ladder = false;
	float ladder_weight = 0.f;
	float ladder_speed = 0.f;

	bool walk_to_run_transition_state = false;

	bool defuse_started = false;
	bool plant_anim_started = false;
	bool twitch_anim_started = false;
	bool adjust_started = false;

	char activity_modifiers_server[20] = {};

	float next_twitch_time = 0.f;

	float time_of_last_known_injury = 0.f;

	float last_velocity_test_time = 0.f;

	vector3d velocity_last = vector3d();
	vector3d target_acceleration = vector3d();
	vector3d acceleration = vector3d();

	float acceleration_weight = 0.f;
 
	float aim_matrix_transition = 0.f;
	float aim_matrix_transition_delay = 0.f;

	bool flashed = false;

	float strafe_change_weight = 0.f;
	float strafe_change_target_weight = 0.f;
	float strafe_change_cycle = 0.f;

	int strafe_sequence = 0;

	bool strafe_changing = false;

	float duration_strafing = 0.f;

	float foot_lerp = 0.f;

	bool feet_crossed = false;

	bool player_is_accelerating = false;

	animstate_pose_param_cache_t pose_param_mappings[player_pose_param_count] = {};

	float duration_move_weight_is_too_high = 0.f;
	float static_approach_speed = 0.f;

	int previous_move_state = 0;
	float stutter_step = 0.f;

	float action_weight_bias_remainder = 0.f;
 
	vector3d foot_left_pos_anim = vector3d();
	vector3d foot_left_pos_anim_last = vector3d();
	vector3d foot_left_pos_plant = vector3d();
	vector3d foot_left_plant_vel = vector3d();

	float foot_left_lock_amount = 0.f;
	float foot_left_last_plant_time = 0.f;

	vector3d foot_right_pos_anim = vector3d();
	vector3d foot_right_pos_anim_last = vector3d();
	vector3d foot_right_pos_plant = vector3d();
	vector3d foot_right_plant_vel = vector3d();

	float foot_right_lock_amount = 0.f;
	float foot_right_last_plant_time = 0.f;

	float camera_smooth_height = 0.f;
	bool smooth_height_valid = false;
	float last_time_velocity_over_ten = 0.f;

	float unk = 0.f;

	float aim_yaw_min = 0.f;
	float aim_yaw_max = 0.f;
	float aim_pitch_min = 0.f;
	float aim_pitch_max = 0.f;

	int animstate_model_version = 0;

	void create();
	void reset();
	void update(const vector3d& angle, bool force = false);

	float get_max_body_rotation();
};