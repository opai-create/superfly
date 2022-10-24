#include "event_visuals.h"

#include "../../../config_vars.h"

#include "../../../../base/sdk.h"
#include "../../../../base/global_variables.h"

#include "../../../../base/tools/math.h"
#include "../../../../base/tools/render.h"

namespace visuals
{
	namespace events
	{
		namespace beams
		{
			void draw_beam(vector3d start, vector3d end, color clr)
			{
				const auto& size = g_cfg.misc.trace_size * 0.1f;
				beam_info_t beam_info;
				beam_info.type = beam_normal;
				beam_info.model_name = "sprites/purplelaser1.vmt";
				beam_info.model_index = -1;
				beam_info.start = start;
				beam_info.end = end;
				beam_info.life = 5.f;
				beam_info.fade_lenght = .1f;
				beam_info.halo_scale = 0.f;
				beam_info.amplitude = 1.f;
				beam_info.segments = 2;
				beam_info.renderable = true;
				beam_info.brightness = clr.a();
				beam_info.red = clr.r();
				beam_info.green = clr.g();
				beam_info.blue = clr.b();
				beam_info.speed = 1.f;
				beam_info.start_frame = 0;
				beam_info.frame_rate = 0.f;
				beam_info.width = size;
				beam_info.end_width = size;
				beam_info.flags = beam_only_no_is_once | beam_notile;

				beam_t* final_beam = i::beam->create_beam_points(beam_info);

				if (final_beam)
					i::beam->draw_beam(final_beam);
			}
		}
	}
}