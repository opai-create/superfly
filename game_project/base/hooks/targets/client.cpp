#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"
#include "../../../functions/config_vars.h"
#include "../../../functions/extra/world_modulation.h"

#include "../../../functions/anti hit/exploits.h"

#include "../../../functions/aimbot/engine_prediction.h"

#include "../../../functions/visuals/visuals.h"
#include "../../../functions/visuals/esp/event/event_visuals.h"
#include "../../../functions/visuals/local/local_player_visuals.h"

#include "../../../functions/hvh/anim_correction.h"
#include "../../../functions/hvh/lag_compensation.h"

#include <string>

namespace tr
{
	namespace client
	{
		void __fastcall frame_stage_notify(void* ecx, void* edx, int stage)
		{
			static auto original = hooker.original(&frame_stage_notify);

			g::in_game = i::engine->is_connected() && i::engine->is_in_game();

			// dynamic intefaces are changing their address ingame
			// so we need to get them when map changes
			i::parse_dynamic_interfaces();

			world_modulation::start(stage);
			visuals::local::draw_fsn(stage);

			visuals::events::hit::client_impact(stage);
			lag_compensation->store_records(stage);

			original(ecx, edx, stage);

			// fix netvar compression by applying predicted vars
			net_compression::set(stage);
		}

		void __fastcall hud_scope_paint(void* ecx, void* edx)
		{
			static auto original = hooker.original(&hud_scope_paint);

			if (!(g_cfg.misc.removals & scope))
				original(ecx, edx);
		}

		void __fastcall draw_models(void* ecx, void* edx, int a2, int a3, int a4, int a5, int a6, char a7)
		{
			static auto original = hooker.original(&draw_models);
			world_modulation::change_clr(a2, a3, a4, a5, a6, a7);
			original(ecx, edx, a2, a3, a4, a5, a6, a7);
		}

		void __fastcall add_view_model_bob(void* ecx, void* edx, void* model, vector3d& origin, vector3d& angles)
		{
			static auto original = hooker.original(&add_view_model_bob);

			if (g_cfg.misc.removals & viewm_bob)
				return;

			original(ecx, edx, model, origin, angles);
		}

		void __stdcall create_move(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
		{
			static auto original = vtables[vtables_t::client].original<create_move_client_fn>(22);

			// ghetto fix for latest csgo update (19.02)
			g::send_packet = &bSendPacket;
			original(i::client, 0, sequence_number, input_sample_frametime, active);
		}

		// push sendpacket to func and then modify it
		__declspec(naked) void __fastcall create_move_wrapper(void* _this, int, int sequence_number, float input_sample_frametime, bool active)
		{
			__asm
			{
				push ebp
				mov  ebp, esp
				push ebx
				push esp
				push dword ptr[active]
				push dword ptr[input_sample_frametime]
				push dword ptr[sequence_number]
				call create_move
				pop  ebx
				pop  ebp
				retn 0Ch
			}
		}


		void __fastcall calc_view_model_bob(void* ecx, void* edx, vector3d& position)
		{
			static auto original = hooker.original(&calc_view_model_bob);

			if (!(g_cfg.misc.removals & land_bob) || ecx != g::local() || !g::local()->is_alive())
				original(ecx, edx, position);
		}

		void on_mapchange()
		{
			visuals::events::hit::reset_info();
			g::update_local = true;
			g::sky_name = i::convar->find_convar("sv_skyname")->string;
		}

		void __fastcall level_init_pre_entity(void* ecx, void* edx, const char* map)
		{
			static auto original = vtables[vtables_t::client].original<level_init_pre_entity_fn>(5);
			on_mapchange();
			original(ecx, map);
		}

		void __fastcall level_init_post_entity(void* ecx, void* edx)
		{
			static auto original = vtables[vtables_t::client].original<level_init_post_entity_fn>(6);
			on_mapchange();
			original(ecx);
		}

		void __fastcall physics_simulate(c_csplayer* ecx, void* edx)
		{
			static auto original = hooker.original(&physics_simulate);

			auto player = reinterpret_cast<c_csplayer*>(ecx);
			auto& simulation_tick = *reinterpret_cast<int*>((uintptr_t)player + 0x2AC);
			auto cctx = reinterpret_cast<c_cmd_ctx*>((uintptr_t)player + 0x34FC);

			if (!g::in_game || !g::local() || !g::local()->is_alive()
				|| i::global_vars->tick_count == simulation_tick
				|| player != g::local()
				|| i::engine->is_playing_demo() || i::engine->is_hltv()
				|| player->flags() & 0x40)
			{
				original(ecx, edx);
				return;
			}

			tick_base.fix(cctx->cmd.command_number, player->tick_base());

			original(ecx, edx);
		}

		void write_user_cmd(void* buf, c_usercmd* in, c_usercmd* out)
		{
			__asm
			{
				mov     ecx, buf
				mov     edx, in
				push    out
				call    game_fn::write_user_cmd
				add     esp, 4
			}
		}

		bool __fastcall write_usercmd_to_delta_buffer(void* ecx, void* edx, int slot, void* buf, int from, int to, bool isnewcommand)
		{
			static auto original = vtables[vtables_t::client].original<write_usercmd_fn>(24);

			if (!g::in_game || !g::local() || !g::local()->is_alive() || exploits::shift_amount <= 0)
				return original(ecx, slot, buf, from, to, isnewcommand);

			if (from != -1)
				return true;

			int _from = -1;

			uintptr_t frame_ptr{};
			__asm mov frame_ptr, ebp;

			int* backup_commands = (int*)(frame_ptr + 0xFD8);
			int* new_commands = (int*)(frame_ptr + 0xFDC);
			int32_t newcmds = *new_commands;

			const auto shift_amt = exploits::shift_amount;

			exploits::shift_amount = 0;
			*backup_commands = 0;

			int choked_modifier = newcmds + shift_amt;

			if (choked_modifier > 62)
				choked_modifier = 62;

			*new_commands = choked_modifier;

			const int next_cmdnr = i::client_state->choked_commands + i::client_state->last_outgoing_command + 1;
			int _to = next_cmdnr - newcmds + 1;
			if (_to <= next_cmdnr)
			{
				while (original(ecx, slot, buf, _from, _to, true))
				{
					_from = _to++;
					if (_to > next_cmdnr)
						goto LABEL_11;
				}
				return false;
			}
		LABEL_11:

			auto* ucmd = i::input->get_user_cmd(_from);
			if (!ucmd)
				return true;

			c_usercmd to_cmd{};
			c_usercmd from_cmd{};

			from_cmd = *ucmd;
			to_cmd = from_cmd;

			++to_cmd.command_number;
			to_cmd.tickcount += g::tick_rate + g::tick_rate * 2;

			if (newcmds > choked_modifier)
				return true;

			for (int i = (choked_modifier - newcmds + 1); i > 0; --i)
			{
				write_user_cmd(buf, &to_cmd, &from_cmd);

				from_cmd = to_cmd;
				++to_cmd.command_number;
				++to_cmd.tickcount;
			}

			exploits::simulation_ticks = choked_modifier - newcmds + 1;

			return true;
		}
	}
}