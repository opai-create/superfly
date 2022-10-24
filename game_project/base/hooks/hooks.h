#pragma once
#include "../../includes.h"
#include "../../base/tools/math.h"

#include "wrapper.h"
#include <unordered_map>

class c_view_setup;
class c_usercmd;
class c_studiohdr;
class c_studio_render;
class vector3d;
class vector4d;
class c_panel;
class c_material;
class c_movehelper;
class c_csplayer;
class c_collideable;
class c_game_trace;
class i_trace_filter;
class c_animstate;

struct draw_model_state_t;
struct model_render_info_t;
struct draw_model_info_t;
struct matrix3x4_t;
struct studio_model_array_info2_t;
struct studio_array_data_t;
struct ray_t;

#pragma region vmt_function_pointers
// client
using level_init_pre_entity_fn = void(__thiscall*)(void*, const char*);
using level_init_post_entity_fn = void(__thiscall*)(void*);
using write_usercmd_fn = bool(__thiscall*)(void*, int, void*, int, int, bool);
using create_move_client_fn = void(__fastcall*)(void*, void*, int, float, bool);

// panel
using paint_traverse_fn = void(__thiscall*)(void*, unsigned int, bool, bool);

// surface
using on_screen_size_changed_fn = void(__thiscall*)(void*, int, int);

// studio render
using draw_model_fn = void(__thiscall*)(c_studio_render*, void*, const draw_model_info_t&,
	matrix3x4_t*, float*, float*, const vector3d&, int);

using draw_model_array_fn = void(__thiscall*)(void*,
	const studio_model_array_info2_t&, int, studio_array_data_t*, int, int);

// client mode
using override_view_fn = void(__thiscall*)(void*, c_view_setup*);
using get_viewmodel_fov_fn = float(__thiscall*)(void*);
using create_move_fn = bool(__thiscall*)(void*, float, c_usercmd*);

// prediction
using in_prediction_fn = bool(__thiscall*)(void*);
using run_command_fn = void(__thiscall*)(void*, c_csplayer*, c_usercmd*, c_movehelper*);

// engine bsp tree query
using list_leaves_in_box_fn = int(__thiscall*)(void*, vector3d&, vector3d&, unsigned short*, int);

// engine
using is_connected_fn = bool(__thiscall*)(void*);
using is_paused_fn = bool(__thiscall*)(void*);
using is_hltv_fn = bool(__thiscall*)(void*);
using get_screen_aspect_ratio_fn = float(__thiscall*)(void*, int, int);

// cl_brushfastpath
using cl_brushfastpath_get_bool_fn = bool(__thiscall*)(void*);

// show_spread
using show_spread_int_fn = int(__thiscall*)(void*);

// engine trace
using clip_ray_to_collideable_fn = void(__thiscall*)(void*, const ray_t&, unsigned int, c_collideable*, c_game_trace*);
using trace_ray_fn = void(__thiscall*)(void*, const ray_t&, unsigned int, i_trace_filter*, c_game_trace*);

// player
using standard_blending_rules_fn = void(__thiscall*)(c_csplayer*, c_studiohdr*, vector3d*, vector4d*, float, int);
using build_transformations_fn = void(__thiscall*)(void*, c_studiohdr*, int, int, int, int, int);
using is_player_fn = bool(__thiscall*)(void*);
using do_extra_bone_processing_fn = void(__thiscall*)(void*, c_studiohdr*, vector3d*, vector4d*,
	const matrix3x4_t&, uint8_t*, void*);
using update_clientside_animation_fn = void(__thiscall*)(void*);
using setup_bones_fn = bool(__thiscall*)(void*, matrix3x4_t*, int, int, float);

// client state
using packet_start_fn = void(__thiscall*)(void*, int, int);
#pragma endregion

enum vtables_t
{
	client = 0,
	panel,
	surface,
	studio_render,
	client_mode,
	prediction,
	engine_bsp,
	engine,
	debug_show_spread,
	trace,
	client_state_,
	vmt_max
};

extern std::array<hooks::vmt::c_hooks, vmt_max> vtables;

extern hooks::detour::c_hooks hooker;

namespace tr
{
	namespace client
	{
		void __fastcall frame_stage_notify(void* ecx, void* edx, int stage);
		void __fastcall hud_scope_paint(void* ecx, void* edx);
		void __fastcall draw_models(void* ecx, void* edx, int a2, int a3, int a4, int a5, int a6, char a7);
		void __fastcall add_view_model_bob(void* ecx, void* edx, void* model, vector3d& origin, vector3d& angles);
		void __fastcall calc_view_model_bob(void* ecx, void* edx, vector3d& position);
		void __fastcall level_init_pre_entity(void* ecx, void* edx, const char* map);
		void __fastcall level_init_post_entity(void* ecx, void* edx);
		void __fastcall physics_simulate(c_csplayer* ecx, void* edx);
		bool __fastcall write_usercmd_to_delta_buffer(void* ecx, void* edx, int slot, void* buf, int from, int to, bool isnewcommand);
		void __fastcall create_move_wrapper(void* _this, int, int sequence_number, float input_sample_frametime, bool active);
	}

	namespace client_mode
	{
		void __fastcall override_view(void* ecx, void* edx, c_view_setup* setup);
		float __fastcall get_viewmodel_fov(void* ecx, void* edx);
		int __fastcall set_view_model_offsets(void* ecx, void* edx, int something, float x, float y, float z);
		bool __fastcall draw_fog(void* ecx, void* edx);
		bool __fastcall create_move(void* ecx, void* edx, float input_sample_frametime, c_usercmd* cmd);
	}

	namespace prediction
	{
		void __fastcall run_command(void* ecx, void* edx, c_csplayer* player, c_usercmd* cmd, c_movehelper* move_helper);
		bool __fastcall in_prediction(void* ecx, void* edx);
	}

	namespace client_state
	{
		void __fastcall packet_start(void* ecx, void* edx, int incoming, int outgoing);
	}

	namespace engine
	{
		int __fastcall list_leaves_in_box(void* ecx, void* edx, vector3d& mins, vector3d& maxs, unsigned short* list, int list_max);
		bool __fastcall using_static_props_debug(void* ecx, void* edx);
		float __fastcall get_screen_aspect_ratio(void* ecx, void* edx, int width, int height);
		void __fastcall check_file_crc_with_server(void* ecx, void* edx);
		bool __fastcall is_connected(void* ecx, void* edx);
		bool __fastcall is_paused(void* ecx, void* edx);
		bool __fastcall is_hltv(void* ecx, void* edx);
	}

	namespace panel
	{
		void __fastcall paint_traverse(c_panel* ecx, void* edx, unsigned int panel, bool a, bool b);
	}

	namespace surface
	{
		void __fastcall on_screen_size_changed(void* ecx, void* edx, int old_w, int old_h);
	}

	// dme chams < studiomodel chams
	// cuz dme calls some code with shadow render
	namespace studio_render
	{
		void __fastcall draw_model(c_studio_render* ecx, void* edx, void* results, const draw_model_info_t& info,
			matrix3x4_t* bone_to_world, float* flex_weights, float* flex_delayed_weights, const vector3d& model_origin, int flags);

		void __fastcall draw_model_array(void* ecx, void* edx,
			const studio_model_array_info2_t& info, int count, studio_array_data_t* array_data, int stride, int flags);
	}

	namespace material_system
	{
		void __fastcall get_color_modulation(c_material* ecx, void* edx, float* r, float* g, float* b);
	}

	namespace input
	{
		bool __fastcall cam_is_thirdperson_overview(void* ecx, void* edx, int slot);
	}

	namespace convars
	{
		int __fastcall debug_show_spread_get_int(void* ecx, void* edx);
	}

	namespace player
	{
		void __fastcall standard_blending_rules(c_csplayer* ecx, void* edx, c_studiohdr* hdr,
			vector3d* pos, vector4d* q, float cur_time, int bone_mask);
		void __fastcall build_transformations(c_csplayer* ecx, void* edx, c_studiohdr* hdr, int a3, int a4, int a5, int a6, int a7);
		bool __fastcall should_skip_anim_frame(c_csplayer* ecx, void* edx);
		void __fastcall do_extra_bone_processing(c_csplayer* ecx, void* edx, c_studiohdr* hdr,
			vector3d* pos, vector4d* q, const matrix3x4_t& mat, uint8_t* bone_computed, void* context);
		void __fastcall update_clientside_animation(c_csplayer* ecx, void* edx);
		void __fastcall modify_eye_position(c_animstate* ecx, void* edx, vector3d& pos);
		bool __fastcall setup_bones(void* ecx, void* edx, matrix3x4_t* bone_to_world, int max_bones, int mask, float time);
	}

	namespace trace
	{
		void __fastcall clip_ray_to_collideable(void* ecx, void* edx, const ray_t& ray, unsigned int mask, c_collideable* collide, c_game_trace* trace);
		void __fastcall trace_ray(void* ecx, void* edx, const ray_t& ray, unsigned int mask, i_trace_filter* filter, c_game_trace* trace);
	}

	LRESULT __stdcall wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
}

namespace hooks
{
	void hook();
	void unhook();
}