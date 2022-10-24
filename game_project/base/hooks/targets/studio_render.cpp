#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"

#include "../../../functions/config_vars.h"
#include "../../../functions/visuals/chams/chams.h"

#include "../../../base/ingame objects/c_csplayer.h"

#include <string>

c_chams chams = {};

namespace tr
{
	namespace studio_render
	{
		void __fastcall draw_model(c_studio_render* ecx, void* edx, void* results, const draw_model_info_t& info,
			matrix3x4_t* bone_to_world, float* flex_weights, float* flex_delayed_weights, const vector3d& model_origin, int flags)
		{
			static auto original = vtables[vtables_t::studio_render].original<draw_model_fn>(29);

			if (flags & render_glow_and_shadow)
				return original(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);

			chams.init(original, ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
			if (!chams.should_render())
				original(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
		}

		void __fastcall draw_model_array(void* ecx, void* edx,
			const studio_model_array_info2_t& info, int count, studio_array_data_t* array_data, int stride, int flags)
		{
			static auto original = vtables[vtables_t::studio_render].original<draw_model_array_fn>(48);

			// 0 flags allows to render opaque and transparent models
			// so we can modify alpha on props and they will be rendered
			original(ecx, info, count, array_data, stride, 0);
		}
	}
}