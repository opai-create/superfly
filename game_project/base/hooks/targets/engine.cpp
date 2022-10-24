#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"

#include "../../../functions/config_vars.h"

#include "../../../base/tools/render.h"

#include "../../../base/ingame objects/c_csplayer.h"
#include "../../../base/ingame objects/c_basecombatweapon.h"

#include "../../../functions/anti hit/fake_lag.h"

namespace tr
{
	namespace client_state
	{
		void __fastcall packet_start(void* ecx, void* edx, int incoming, int outgoing) 
		{
			static auto original = vtables[vtables_t::client_state_].original<packet_start_fn>(5);

			if (!g::local() || !g::local()->is_alive())
				return original(ecx, incoming, outgoing);

			if (fake_lag::commands.empty())
				return original(ecx, incoming, outgoing);

			for (auto it = fake_lag::commands.rbegin(); it != fake_lag::commands.rend(); ++it)
			{
				if (!it->is_outgoing)
					continue;

				if (it->cmd_number == outgoing || outgoing > it->cmd_number && (!it->is_used || it->previous_command_number == outgoing))
				{
					it->previous_command_number = outgoing;
					it->is_used = true;
					original(ecx, incoming, outgoing);
					break;
				}
			}

			auto result = false;

			for (auto it = fake_lag::commands.begin(); it != fake_lag::commands.end();)
			{
				if (outgoing == it->cmd_number || outgoing == it->previous_command_number)
					result = true;

				if (outgoing > it->cmd_number && outgoing > it->previous_command_number)
					it = fake_lag::commands.erase(it);
				else
					++it;
			}

			if (!result)
				original(ecx, incoming, outgoing);
		}
	}

	namespace engine
	{
		vector3d bounds_min = vector3d(-32768.f, -32768.f, -32768.f);
		vector3d bounds_max = vector3d(32768.f, 32768.f, 32768.f);

		int __fastcall list_leaves_in_box(void* ecx, void* edx, vector3d& mins, vector3d& maxs, unsigned short* list, int list_max)
		{
			static auto original = vtables[vtables_t::engine_bsp].original<list_leaves_in_box_fn>(6);

			if ((uintptr_t)_ReturnAddress() != patterns[pattern_t::list_leaves_in_box].as<uintptr_t>())
				return original(ecx, mins, maxs, list, list_max);

			auto info = *(renderable_info**)((uintptr_t)_AddressOfReturnAddress() + 0x14);

			if (!info)
				return original(ecx, mins, maxs, list, list_max);

			if (!info->m_renderable)
				return original(ecx, mins, maxs, list, list_max);

			c_baseentity* entity = info->m_renderable->get_i_unknown_entity()->get_base_entity();

			if (!entity || !entity->is_player())
				return original(ecx, mins, maxs, list, list_max);

			info->m_flags &= ~0x100;
			info->m_flags2 |= 0xC0;

			return original(ecx, bounds_min, bounds_max, list, list_max);
		}

		bool __fastcall using_static_props_debug(void* ecx, void* edx)
		{
			return false;
		}

		float __fastcall get_screen_aspect_ratio(void* ecx, void* edx, int width, int height)
		{
			static auto original = vtables[vtables_t::engine].original<get_screen_aspect_ratio_fn>(101);

			float aspect_ratio = render::screen.w / render::screen.h;

			return g_cfg.misc.aspect_ratio > 0 ?
				(aspect_ratio * g_cfg.misc.aspect_ratio) / 100.f
				: original(ecx, width, height);
		}

		void __fastcall check_file_crc_with_server(void* ecx, void* edx)
		{
			return;
		}

		bool __fastcall is_connected(void* ecx, void* edx)
		{
			static auto original = vtables[vtables_t::engine].original<is_connected_fn>(27);

			/*if ((uintptr_t)_ReturnAddress() == patterns[pattern_t::inventory_access].as<uintptr_t>() && g_cfg.misc.unlock_inventory)
				return false;*/

			return original(ecx);
		}

		bool __fastcall is_paused(void* ecx, void* edx)
		{
			static auto original = vtables[vtables_t::engine].original<is_paused_fn>(90);

			if ((uintptr_t)_ReturnAddress() == patterns[retn_to_extrapolation].as<uintptr_t>())
				return true;

			return original(ecx);
		}

		bool __fastcall is_hltv(void* ecx, void* edx)
		{
			static auto original = vtables[vtables_t::engine].original<is_hltv_fn>(93);

			if ((uintptr_t)_ReturnAddress() == patterns[retn_to_setup_velocity].as<uintptr_t>()
				|| (uintptr_t)_ReturnAddress() == patterns[retn_to_accumulate_layers].as<uintptr_t>()
				|| (uintptr_t)_ReturnAddress() == patterns[retn_to_reevaluate_anim_lod].as<uintptr_t>())
				return true;

			return original(ecx);
		}
	}
}