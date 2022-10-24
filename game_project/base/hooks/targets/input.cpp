#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"

#include "../../../functions/config_vars.h"

namespace tr
{
	namespace input
	{
		bool __fastcall cam_is_thirdperson_overview(void* ecx, void* edx, int slot)
		{
			static auto original = hooker.original(&cam_is_thirdperson_overview);

			if (g_cfg.misc.nightmode)
			{
				// disabling dynamic alpha prop range
				if ((uintptr_t)_ReturnAddress() == patterns[return_addr_compute_translucency].as<uintptr_t>())
					return true;
			}

			return original(ecx, edx, slot);
		}
	}
}