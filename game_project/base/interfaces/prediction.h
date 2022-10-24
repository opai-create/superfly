#pragma once
#include "../tools/memory.h"
#include "../tools/math.h"

#include "../../base/ingame objects/c_usercmd.h"

class c_client_entity;
class c_csplayer;

class c_movehelper
{
public:
	virtual void unk_virtual() = 0;
	virtual void set_host(c_client_entity* host) = 0;
private:
	virtual void pad00() = 0;
	virtual void pad01() = 0;
public:
	virtual void process_impacts() = 0;
};

class c_movedata
{
public:
	bool run_out_of_functions : 1;
	bool game_code_moved_player : 1;
	int player_handle;
	int impulse_command;
	vector3d view_angles;
	vector3d abs_view_angles;
	int buttons;
	int old_buttons;
	float forward_move;
	float side_move;
	float up_move;
	float max_speed;
	float client_max_speed;
	vector3d velocity;
	vector3d angles;
	vector3d old_angle;
	float step_height;
	vector3d wish_vel;
	vector3d jump_vel;
	vector3d constraint_center;
	float constraint_radius;
	float constraint_width;
	float constraint_speed_factor;
	float unknown_float_array[5];
	vector3d abs_origin;
};

class c_game_movement
{
public:
	virtual ~c_game_movement(void) { }

	virtual void process_movement(c_csplayer* player, c_movedata* move_data) = 0;
	virtual void reset(void) = 0;
	virtual void start_track_prediction_errors(c_csplayer* player) = 0;
	virtual void finish_track_prediction_errors(c_csplayer* player) = 0;
	virtual void diff_print(char const* fmt, ...) = 0;

	virtual vector3d const& get_player_mins(bool ducked) const = 0;
	virtual vector3d const& get_player_maxs(bool ducked) const = 0;
	virtual vector3d const& get_player_view_offset(bool ducked) const = 0;

	virtual bool is_moving_player_stuck(void) const = 0;
	virtual c_csplayer* get_moving_player(void) const = 0;
	virtual void ublock_pusher(c_csplayer* player, c_csplayer* pusher) = 0;

	virtual void setup_movement_bounds(c_movedata* move_data) = 0;
};

class c_prediction
{
public:
	void update(int start_frame, bool valid_frame, int inc_ack, int out_cmd)
	{
		using fn = void(__thiscall*)(void*, int, bool, int, int);
		return memory::vfunc<fn>(this, 3)(this, start_frame, valid_frame, inc_ack, out_cmd);
	}

	void check_moving_ground(c_csplayer* player, float frame_time)
	{
		using fn = void(__thiscall*)(void*, c_csplayer*, float);
		return memory::vfunc<fn>(this, 18)(this, player, frame_time);
	}

	void run_command(c_csplayer* player, c_csplayer* cmd, c_movehelper* move_helper)
	{
		using fn = void(__thiscall*)(void*, c_csplayer*, c_csplayer*, c_movehelper*);
		return memory::vfunc<fn>(this, 19)(this, player, cmd, move_helper);
	}

	void setup_move(c_csplayer* player, c_usercmd* cmd, c_movehelper* move_helper, void* move_data)
	{
		using fn = void(__thiscall*)(void*, c_csplayer*, c_usercmd*, c_movehelper*, void*);
		return memory::vfunc<fn>(this, 20)(this, player, cmd, move_helper, move_data);
	}

	void finish_move(c_csplayer* player, c_usercmd* cmd, void* move_data)
	{
		using fn = void(__thiscall*)(void*, c_csplayer*, c_usercmd*, void*);
		return memory::vfunc<fn>(this, 21)(this, player, cmd, move_data);
	}

	void set_local_view_angles(vector3d& angles)
	{
		using fn = void(__thiscall*)(void*, vector3d&);
		return memory::vfunc<fn>(this, 13)(this, angles);
	}

	int last_ground;

	bool in_prediction;
	bool is_first_time_predicted;

	bool engine_paused;
	bool old_cl_predict_value;

	int previous_startframe;
	int incoming_packet_number;

	char pad[0x4];

	int commands_predicted;
	int server_commands_acknowledged;
	bool prev_ack_had_errors;
};