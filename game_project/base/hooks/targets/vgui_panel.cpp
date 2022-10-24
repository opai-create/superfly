#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"

#include "../../../base/tools/render.h"

#include "../../../functions/config_vars.h"
#include "../../../functions/visuals/visuals.h"

namespace tr
{
	namespace panel
	{
		void __fastcall paint_traverse(c_panel* ecx, void* edx, unsigned int panel, bool a, bool b)
		{
			static auto original = vtables[vtables_t::panel].original<paint_traverse_fn>(41);

			if (!std::strcmp(ecx->get_name(panel), "FocusOverlayPanel"))
				ecx->set_mouse_input_enabled(panel, g::uninject ? false : g_cfg.misc.menu);

			original(ecx, panel, a, b);
			visuals::on_paint_traverse(ecx, panel);
		}
	}
}