#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"

#include "../../../functions/extra/world_modulation.h"

#include <string>

namespace tr
{
	namespace material_system
	{
		void __fastcall get_color_modulation(c_material* ecx, void* edx, float* r, float* g, float* b)
		{
			static auto original = hooker.original(&get_color_modulation);
			original(ecx, edx, r, g, b);
			world_modulation::change_world_clr(ecx, r, g, b);
		}
	}
}