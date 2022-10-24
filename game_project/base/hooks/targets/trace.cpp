#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"
#include "../../../functions/config_vars.h"
#include "../../../functions/extra/world_modulation.h"

#include "../../../functions/visuals/visuals.h"
#include "../../../functions/visuals/esp/event/event_visuals.h"
#include "../../../functions/visuals/local/local_player_visuals.h"

#include "../../../functions/hvh/anim_correction.h"
#include "../../../functions/hvh/lag_compensation.h"

#include <string>

namespace tr
{
	namespace trace
	{
		void __fastcall clip_ray_to_collideable(void* ecx, void* edx, const ray_t& ray, unsigned int mask, c_collideable* collide, c_game_trace* trace)
		{
			static auto original = vtables[vtables_t::trace].original<clip_ray_to_collideable_fn>(4);

			//auto old_obb = collide->get_maxs();
			//collide->get_maxs().z += 5.f;
			original(ecx, ray, mask, collide, trace);
			//collide->get_maxs() = old_obb;
		}

		void __fastcall trace_ray(void* ecx, void* edx, const ray_t& ray, unsigned int mask, i_trace_filter* filter, c_game_trace* trace)
		{
			static auto original = vtables[vtables_t::trace].original<trace_ray_fn>(5);

		/*	if (!g::autowalling)
				return original(ecx, ray, mask, filter, trace);*/

			original(ecx, ray, mask, filter, trace);
			//trace->surface.flags |= surf_sky;
		}
	}
}