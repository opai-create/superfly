#include "../includes.h"

#include "sdk.h"
#include "global_variables.h"

#include "tools/memory.h"

#include "ingame objects/c_csplayer.h"
#include "ingame objects/c_csplayerresource.h"

std::array<Address, pattern_max> patterns = {};

namespace i
{
	c_base_client_dll* client = nullptr;
	c_global_vars_base* global_vars = nullptr;
	c_surface* surface = nullptr;
	c_panel* panel = nullptr;
	c_engine* engine = nullptr;
	c_entity_list* entity_list = nullptr;
	c_debug_overlay* debug_overlay = nullptr;
	c_input_system* input_system = nullptr;
	c_input* input = nullptr;
	c_engine_trace* engine_trace = nullptr;
	c_model_render* model_render = nullptr;
	c_material_system* material_system = nullptr;
	c_studio_render* studio_render = nullptr;
	c_localize* localize = nullptr;
	c_convar* convar = nullptr;
	c_prediction* prediction = nullptr;
	c_client_mode* client_mode = nullptr;
	c_movehelper* move_helper = nullptr;
	c_game_movement* game_movement = nullptr;
	c_game_event_manager2* game_event_manager = nullptr;
	c_view_render_beams* beam = nullptr;
	c_csplayer_resource* player_resource = nullptr;
	c_memory_allocate* memory = nullptr;
	c_clientstate* client_state = nullptr;
	c_model_info* model_info = nullptr;
	c_phys_surface_props* phys_surface_props = nullptr;

	bool should_change = true;
	void parse_dynamic_interfaces()
	{
		if (!g::in_game)
		{
			if (!should_change)
			{
				log_console("Reset dynamic interfaces...\n");
				should_change = true;
			}
			return;
		}

		if (!should_change)
			return;

		log_console("Parsing dynamic interfaces...\n");

		log_console("Interfaces: player resource : Module: client.dll \n");
		player_resource = **patterns[pattern_t::player_resource].as<c_csplayer_resource***>();

		log_console("  \n");

		should_change = false;
	}

	void parse()
	{
		log_console("Parsing Interfaces...\n");

		client = (c_base_client_dll*)memory::get_interface("client.dll", "VClient018");
		global_vars = **(c_global_vars_base***)(memory::vfunc<DWORD>(client, 0) + 0x1F);
		surface = (c_surface*)memory::get_interface("vguimatsurface.dll", "VGUI_Surface031");
		panel = (c_panel*)memory::get_interface("vgui2.dll", "VGUI_Panel009");
		engine = (c_engine*)memory::get_interface("engine.dll", "VEngineClient014");
		entity_list = (c_entity_list*)memory::get_interface("client.dll", "VClientEntityList003");
		debug_overlay = (c_debug_overlay*)memory::get_interface("engine.dll", "VDebugOverlay004");
		input_system = (c_input_system*)memory::get_interface("inputsystem.dll", "InputSystemVersion001");
		engine_trace = (c_engine_trace*)memory::get_interface("engine.dll", "EngineTraceClient004");
		model_render = (c_model_render*)memory::get_interface("engine.dll", "VEngineModel016");
		material_system = (c_material_system*)memory::get_interface("materialsystem.dll", "VMaterialSystem080");
		studio_render = (c_studio_render*)memory::get_interface("studiorender.dll", "VStudioRender026");
		localize = (c_localize*)memory::get_interface("localize.dll", "Localize_001");
		convar = (c_convar*)memory::get_interface("vstdlib.dll", "VEngineCvar007");
		game_movement = (c_game_movement*)memory::get_interface("client.dll", "GameMovement001");
		prediction = (c_prediction*)memory::get_interface("client.dll", "VClientPrediction001");
		game_event_manager = (c_game_event_manager2*)memory::get_interface("engine.dll", "GAMEEVENTSMANAGER002");
		model_info = (c_model_info*)memory::get_interface("engine.dll", "VModelInfoClient004");
		phys_surface_props = (c_phys_surface_props*)memory::get_interface("vphysics.dll", "VPhysicsSurfaceProps001");

		log_console("Interface: input | Module: client.dll \n");
		input = *patterns[pattern_t::input].as<c_input**>();

		log_console("Interface: client mode | Module: client.dll \n");
		client_mode = **(c_client_mode***)(memory::vfunc<DWORD>(client, 10) + 0x5);

		log_console("Interface: move helper | Module: client.dll \n");
		move_helper = **patterns[pattern_t::move_helper].as<c_movehelper***>();

		log_console("Interface: render beams | Module: client.dll \n");
		beam = *patterns[pattern_t::beam].as<c_view_render_beams**>();

		log_console("Interface: memalloc | Module: tier0.dll \n");
		memory = *PE::GetExport(m::tier0, HASH("g_pMemAlloc")).as<c_memory_allocate**>();

		log_console("Interface: client state | Module: engine.dll \n");
		client_state = **patterns[pattern_t::client_state].add(1).as<c_clientstate***>();

		log_console(" \n");
	}
}

namespace g
{
	bool uninject = false;
	bool in_game = false;

	std::array< key_t, 256 > keys = {};

	HWND window = NULL;
	WNDPROC backup_window = NULL;

	c_csplayer* local()
	{
		return (c_csplayer*)i::entity_list->get_entity(i::engine->get_local_player());
	}

	c_basecombatweapon* weapon()
	{
		return local()->get_active_weapon();
	}

	c_usercmd* cmd = nullptr;
	bool* send_packet = nullptr;

	vector3d cmd_angle = vector3d();
	vector3d base_angle = vector3d();

	int sent_tick_count = 0;
	int hitchance_seed = 0;

	float abs_rotation = 0.f;

	alignas(16) matrix3x4_t matrix[128] = {};

	bool init_seed = false;
	bool setup_bones = false;
	bool update_local = true;
	bool update_enemy = true;

	bool autowalling = false;
	bool modify_eye_pos = false;

	int tick_rate = 0;
	int tick_base = 0;

	std::string sky_name = "";
}