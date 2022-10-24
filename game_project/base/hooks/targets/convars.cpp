#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"
#include "../../../functions/config_vars.h"

#include "../../../base/ingame objects/c_csplayer.h"
#include "../../../base/ingame objects/c_basecombatweapon.h"

namespace tr
{
	namespace convars
	{
		int __fastcall debug_show_spread_get_int(void* ecx, void* edx)
		{
			static auto original = vtables[vtables_t::debug_show_spread].original<show_spread_int_fn>(13);

			if (!g::local() || !g::local()->is_alive())
				return original(ecx);

			c_basecombatweapon* weapon = g::weapon();
			if (!weapon)
				return original(ecx);

			if (!g_cfg.misc.snip_crosshair || g::local()->is_scoped())
				return original(ecx);

			if (!weapon || !weapon->is_sniper())
				return original(ecx);

			return 3;
		}
	}
}