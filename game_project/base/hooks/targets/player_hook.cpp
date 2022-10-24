#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"

#include "../../../base/ingame objects/c_csplayer.h"

#include "../../../functions/hvh/anim_correction.h"

namespace tr
{
	namespace player
	{
		// disabling bone interpolation
		void __fastcall standard_blending_rules(c_csplayer* ecx, void* edx, c_studiohdr* hdr, vector3d* pos, vector4d* q, float cur_time, int bone_mask)
		{
			static auto original = hooker.original(&standard_blending_rules);
			if (i::client_state->delta_tick == -1 || g::uninject || !ecx->is_player() || !ecx->is_alive() || !std::isfinite(cur_time))
				return original(ecx, edx, hdr, pos, q, cur_time, bone_mask);

			ecx->disable_interpolation();
			original(ecx, edx, hdr, pos, q, cur_time, bone_mask);
			ecx->enable_interpolation();
		}

		void __fastcall build_transformations(c_csplayer* ecx, void* edx, c_studiohdr* hdr, int a3, int a4, int a5, int a6, int a7)
		{
			static auto original = hooker.original(&build_transformations);
			if (i::client_state->delta_tick == -1 || g::uninject || !ecx->is_player() || !ecx->is_alive())
				return original(ecx, edx, hdr, a3, a4, a5, a6, a7);

			auto old_jiggle_bones = ecx->jiggle_bones_enabled();
			ecx->jiggle_bones_enabled() = false;
			original(ecx, edx, hdr, a3, a4, a5, a6, a7);
			ecx->jiggle_bones_enabled() = old_jiggle_bones;
		}

		// allow game to setup bones every frame
		bool __fastcall should_skip_anim_frame(c_csplayer* ecx, void* edx)
		{
			static auto original = hooker.original(&should_skip_anim_frame);
			if (i::client_state->delta_tick == -1 || g::uninject || !ecx->is_player() || !ecx->is_alive())
				return original(ecx, edx);

			return false;
		}

		// disable leg rotations
		void __fastcall do_extra_bone_processing(c_csplayer* ecx, void* edx, c_studiohdr* hdr, vector3d* pos, vector4d* q, const matrix3x4_t& mat, uint8_t* bone_computed, void* context)
		{
			static auto original = hooker.original(&do_extra_bone_processing);
			if (i::client_state->delta_tick == -1 || g::uninject || !ecx->is_player() || !ecx->is_alive())
				return original(ecx, edx, hdr, pos, q, mat, bone_computed, context);

			return;
		}

		void __fastcall update_clientside_animation(c_csplayer* ecx, void* edx)
		{
			static auto original = hooker.original(&update_clientside_animation);
			if (i::client_state->delta_tick == -1 || g::uninject || !ecx->is_player() || !ecx->is_alive())
				return original(ecx, edx);

			if (g::local() == ecx)
			{
				if (g::update_local)
					return original(ecx, edx);

				
			}
			else
				original(ecx, edx);
		}

		void __fastcall modify_eye_position(c_animstate* ecx, void* edx, vector3d& pos)
		{
			static auto original = hooker.original(&modify_eye_position);

			original(ecx, edx, pos);
		}

		bool __fastcall setup_bones(void* ecx, void* edx, matrix3x4_t* bone_to_world, int max_bones, int mask, float time)
		{
			static auto original = hooker.original(&setup_bones);

			auto player = (c_csplayer*)((uintptr_t)ecx - 4);

			if (i::client_state->delta_tick == -1 || g::uninject || !player || !player->is_player() || !player->is_alive())
				return original(ecx, edx, bone_to_world, max_bones, mask, time);

			if (player != g::local())
				return original(ecx, edx, bone_to_world, max_bones, mask, time);

			if (g::setup_bones || !anim_correction::bones.bones)
				return original(ecx, edx, bone_to_world, max_bones, mask, time);

			if (bone_to_world) {
				auto orig = player->get_render_origin();
				for (auto& i : g::matrix) {
					i.mat[0][3] += orig.x;
					i.mat[1][3] += orig.y;
					i.mat[2][3] += orig.z;
				}

				std::memcpy(bone_to_world, g::matrix, sizeof(matrix3x4_t) * player->get_bone_cache().count());
				game_fn::attachments_helper(player, player->get_studio_hdr());

				for (auto& i : g::matrix) {
					i.mat[0][3] -= orig.x;
					i.mat[1][3] -= orig.y;
					i.mat[2][3] -= orig.z;
				}
			}

			return true;
		}
	}
}