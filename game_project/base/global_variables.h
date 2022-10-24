#pragma once
#include <array>
#include <string>
#include <Windows.h>
#include <corecrt_math_defines.h>

struct key_t
{
	bool down;
	bool pressed;
	int  tick;
	int  old_tick;
};

class c_base_client_dll;
class c_global_vars_base;
class c_surface;
class c_panel;
class c_engine;
class c_entity_list;
class c_client_mode;
class c_debug_overlay;
class c_input_system;
class c_input;
class c_engine_trace;
class c_model_render;
class c_material_system;
class c_studio_render;
class c_localize;
class c_convar;
class c_prediction;
class c_movehelper;
class c_game_movement;
class c_game_event_manager2;
class c_view_render_beams;
class c_csplayer_resource;
class c_memory_allocate;
class c_clientstate;
class c_model_info;
class c_phys_surface_props;

class c_csplayer;
class c_basecombatweapon;

class c_usercmd;

class vector3d;

struct matrix3x4_t;

namespace i
{
	extern c_base_client_dll* client;
	extern c_global_vars_base* global_vars;
	extern c_surface* surface;
	extern c_panel* panel;
	extern c_engine* engine;
	extern c_entity_list* entity_list;
	extern c_debug_overlay* debug_overlay;
	extern c_input_system* input_system;
	extern c_input* input;
	extern c_engine_trace* engine_trace;
	extern c_model_render* model_render;
	extern c_material_system* material_system;
	extern c_studio_render* studio_render;
	extern c_localize* localize;
	extern c_prediction* prediction;
	extern c_convar* convar;
	extern c_client_mode* client_mode;
	extern c_movehelper* move_helper;
	extern c_game_movement* game_movement;
	extern c_game_event_manager2* game_event_manager;
	extern c_view_render_beams* beam;
	extern c_csplayer_resource* player_resource;
	extern c_memory_allocate* memory;
	extern c_clientstate* client_state;
	extern c_model_info* model_info;
	extern c_phys_surface_props* phys_surface_props;

	void parse();
	void parse_dynamic_interfaces();
}

namespace g
{
	extern bool uninject;

	extern bool in_game;

	extern std::array< key_t, 256 > keys;

	extern HWND window;
	extern WNDPROC backup_window;

	extern c_csplayer* local();

	extern c_basecombatweapon* weapon();

	extern c_usercmd* cmd;
	extern bool* send_packet;

	extern vector3d cmd_angle;
	extern vector3d base_angle;

	extern int sent_tick_count;
	extern int hitchance_seed;

	extern float abs_rotation;

	extern alignas(16) matrix3x4_t matrix[128];

	extern bool choke_shot;

	extern bool init_seed;
	extern bool setup_bones;

	extern bool update_local;
	extern bool update_enemy;

	extern bool autowalling;
	extern bool modify_eye_pos;

	extern int tick_rate;
	extern int tick_base;

	extern std::string sky_name;
}