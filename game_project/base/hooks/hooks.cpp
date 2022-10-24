#include "hooks.h"

#include "../../base/sdk.h"
#include "../../base/global_variables.h"
#include "../../base/tools/memory.h"

#include "../../functions/listeners/listener_entity.h"

std::array<hooks::vmt::c_hooks, vmt_max> vtables = {};
hooks::detour::c_hooks hooker = {};

void init_vtables()
{
	log_console("Initalizing Vtables....\n");

	c_cvar* show_spread = i::convar->find_convar("weapon_debug_spread_show");

	vtables[vtables_t::client].setup(i::client);
	vtables[vtables_t::panel].setup(i::panel);
	vtables[vtables_t::surface].setup(i::surface);
	vtables[vtables_t::studio_render].setup(i::studio_render);
	vtables[vtables_t::client_mode].setup(i::client_mode);
	vtables[vtables_t::prediction].setup(i::prediction);
	vtables[vtables_t::engine_bsp].setup(i::engine->get_bsp_tree_query());
	vtables[vtables_t::engine].setup(i::engine);
	vtables[vtables_t::debug_show_spread].setup(show_spread);
	vtables[vtables_t::trace].setup(i::engine_trace);
	vtables[vtables_t::client_state_].setup((c_clientstate*)((uint32_t)i::client_state + 0x8));

	log_console(" \n");
}

void hook_vtable()
{
	log_console("Hooking VMT functions....\n");

	log_console("VMT Table from: Panel \n");
	vtables[vtables_t::panel].hook(41, tr::panel::paint_traverse);
	log_console(" \n");

	log_console("VMT Table from: Client \n");
	vtables[vtables_t::client].hook(5, tr::client::level_init_pre_entity);
	vtables[vtables_t::client].hook(6, tr::client::level_init_post_entity);
	vtables[vtables_t::client].hook(24, tr::client::write_usercmd_to_delta_buffer);
	vtables[vtables_t::client].hook(22, tr::client::create_move_wrapper);
	log_console(" \n");

	log_console("VMT Table from: Surface \n");
	vtables[vtables_t::surface].hook(116, tr::surface::on_screen_size_changed);
	log_console(" \n");

	log_console("VMT Table from: Studio Render \n");
	vtables[vtables_t::studio_render].hook(29, tr::studio_render::draw_model);
	vtables[vtables_t::studio_render].hook(48, tr::studio_render::draw_model_array);
	log_console(" \n");

	log_console("VMT Table from: Client Mode \n");
	vtables[vtables_t::client_mode].hook(18, tr::client_mode::override_view);
	vtables[vtables_t::client_mode].hook(35, tr::client_mode::get_viewmodel_fov);
	vtables[vtables_t::client_mode].hook(24, tr::client_mode::create_move);
	log_console(" \n");

	
	log_console("VMT Table from: Prediction \n");
	vtables[vtables_t::prediction].hook(14, tr::prediction::in_prediction);
	vtables[vtables_t::prediction].hook(19, tr::prediction::run_command);
	log_console(" \n");

	log_console("VMT Table from: Engine::GetBSPTreeQuery \n");
	vtables[vtables_t::engine_bsp].hook(6, tr::engine::list_leaves_in_box);
	log_console(" \n");

	log_console("VMT Table from: Engine \n");
	vtables[vtables_t::engine].hook(27, tr::engine::is_connected);
	vtables[vtables_t::engine].hook(90, tr::engine::is_paused);
	vtables[vtables_t::engine].hook(93, tr::engine::is_hltv);
	vtables[vtables_t::engine].hook(101, tr::engine::get_screen_aspect_ratio);
	log_console(" \n");

	log_console("VMT Table from: weapon_debug_show_spread \n");
	vtables[vtables_t::debug_show_spread].hook(13, tr::convars::debug_show_spread_get_int);
	log_console(" \n");

	log_console("VMT Table from: Engine Trace \n");
	//vtables[vtables_t::trace].hook(4, tr::trace::clip_ray_to_collideable);
	vtables[vtables_t::trace].hook(5, tr::trace::trace_ray);

	log_console("VMT Table from: Client State \n");
	vtables[vtables_t::client_state_].hook(5, tr::client_state::packet_start);
	log_console(" \n");
}

void hook_funcs()
{
	log_console("Hooking functions by MinHook....\n");

	log_console("Hook target: Client \n");
	{
		void* fsn_vfunc = memory::vfunc(i::client, 37);

		log_console("Function: FrameStageNotify \n");
		hooker.create_hook(tr::client::frame_stage_notify, fsn_vfunc);

		log_console("Function: HudScopePaint \n");
		hooker.create_hook(tr::client::hud_scope_paint, patterns[hud_scope_paint].as<void*>());

		log_console("Function: DrawModels \n");
		hooker.create_hook(tr::client::draw_models, patterns[draw_models].as<void*>());

		log_console("Function: AddViewModelBob \n");
		hooker.create_hook(tr::client::add_view_model_bob, patterns[add_view_model_bob].as<void*>());

		log_console("Function: CalcViewModelBob \n");
		hooker.create_hook(tr::client::calc_view_model_bob, patterns[calc_view_model_bob].as<void*>());

		log_console("Function: PhysicsSimulate \n");
		hooker.create_hook(tr::client::physics_simulate, patterns[physics_simulate].as<void*>());
	}
	log_console(" \n");

	log_console("Hook target: Client Mode \n");
	{
		log_console("Function: SetViewModelOffsets \n");
		hooker.create_hook(tr::client_mode::set_view_model_offsets, patterns[set_view_model_offsets].as<void*>());

		log_console("Function: ShouldDrawFog \n");
		hooker.create_hook(tr::client_mode::draw_fog, patterns[remove_fog].as<void*>());
	}
	log_console(" \n");

	log_console("Hook target: Engine \n");
	{
		log_console("Function: UsingStaticPropDebug \n");
		hooker.create_hook(tr::engine::using_static_props_debug, patterns[using_static_prop_debug].as<void*>());

		log_console("Function: CheckCRCFileWithServer \n");
		hooker.create_hook(tr::engine::check_file_crc_with_server, patterns[check_file_crc_with_server].as<void*>());
	}
	log_console(" \n");

	log_console("Hook target: Material System \n");
	{
		log_console("Function: GetColorModulation \n");
		hooker.create_hook(tr::material_system::get_color_modulation, patterns[get_color_modulation].as<void*>());
	}
	log_console(" \n");

	log_console("Hook target: Input \n");
	{
		log_console("Function: CamIsThirdpersonOverview \n");
		hooker.create_hook(tr::input::cam_is_thirdperson_overview, patterns[is_thirdperson_overview].as<void*>());
	}
	log_console(" \n");

	log_console("Hook target: C_BasePlayer and related vtables \n");
	{
		log_console("Function: ShouldSkipAnimFrame \n");
		hooker.create_hook(tr::player::should_skip_anim_frame, patterns[should_skip_anim_frame].as<void*>());

		/*log_console("Function: ModifyEyePosition \n");
		hooker.create_hook(tr::player::modify_eye_position, patterns[modify_eye_position].as<void*>());*/

		log_console("Function: SetupBones \n");
		hooker.create_hook(tr::player::setup_bones, patterns[setup_bones].as<void*>());

		log_console("Function: BuildTransformations \n");
		hooker.create_hook(tr::player::build_transformations, patterns[build_transformations].as<void*>());

		log_console("Function: StandardBlendingRules \n");
		hooker.create_hook(tr::player::standard_blending_rules, patterns[standard_blending_rules].as<void*>());

		log_console("Function: DEBP \n");
		hooker.create_hook(tr::player::do_extra_bone_processing, patterns[do_extra_bone_processing].as<void*>());

		log_console("Function: UpdateClientsideAnimation \n");
		hooker.create_hook(tr::player::update_clientside_animation, patterns[update_clientside_animation].as<void*>());
	}
	log_console(" \n");

	hooker.enable();
}

__inline void hook_winapi()
{
	log_console("Hooking WinApi functions... \n");

	log_console("Function: WndProc \n");

	if (g::window)
		g::backup_window = (WNDPROC)SetWindowLongPtr(g::window, GWL_WNDPROC, (LONG_PTR)tr::wnd_proc);

	log_console(" \n");
}

namespace hooks
{
	void hook()
	{
		hook_winapi();

		init_vtables();
		hook_vtable();

		hook_funcs();
	}

	void unhook()
	{
		SetWindowLongPtr(g::window, GWL_WNDPROC, (LONG_PTR)g::backup_window);

		for (auto v : vtables)
			v.unhook_all();

		hooker.restore();
	}
}