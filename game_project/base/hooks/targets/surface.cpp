#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"

#include "../../../base/tools/render.h"

namespace tr
{
	namespace surface
	{
		void __fastcall on_screen_size_changed(void* ecx, void* edx, int old_w, int old_h)
		{
			static auto original = vtables[vtables_t::surface].original<on_screen_size_changed_fn>(116);
			original(ecx, old_w, old_h);
			render::init();
		}
	}
}