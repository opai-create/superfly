#include "anim_correction.h"
#include "setup_bones_manager.h"

namespace anim_correction {
	void force_update(c_csplayer* player, bool lag, bool animstate, const vector3d& angle) {
		if (!lag)
			return;

		auto state = player->get_animation_state();
		if (!state)
			return;

		// now we can clear anim lod and reset dispatch
		// cuz it happens when enemy change his weapon
		state->weapon_last = player->get_active_weapon();

		for (int i = 0; i < 13; i++) {
			auto layer = &player->get_animation_layer()[i];
			layer->owner = player;
			layer->studio_hdr = player->get_studio_hdr();
		}

		if (!animstate) {
			float old_cur_time = i::global_vars->cur_time;
			float old_frame_time = i::global_vars->frame_time;

			i::global_vars->cur_time = player->simulation_time();
			i::global_vars->frame_time = i::global_vars->interval_per_tick;

			if (state->last_update_frame == i::global_vars->frame_count)
				--state->last_update_frame;

			player->clientside_anim() = true;
			player->update_clientside_animation();
			player->clientside_anim() = false;

			i::global_vars->cur_time = old_cur_time;
			i::global_vars->frame_time = old_frame_time;
		}
		else
			state->update(angle);

		game_fn::invalidate_physics_recursive(player, 0xA);
	}

	void update_viewmodel() {
		int get_weapon = memory::vfunc<int(__thiscall*)(c_csplayer*)>(g::local(), 268)(g::local());
		if (get_weapon == 0)
			return;

		int viewmodel_idx = *(int*)(get_weapon + 0x3224);

		void* view_model = game_fn::get_viewmodel(g::local(), viewmodel_idx);
		if (!view_model)
			return;

		game_fn::update_all_viewmodel_addons(view_model);
	}

	c_animationlayer layers[13] = {};
	std::array<float, 24> poses = {};

	c_setupbones bones = {};

	// TO-DO: fix server-delay animations
	void update_render_data() {
		if (i::client_state->delta_tick == -1)
			return;

		c_animstate* animstate = g::local()->get_animation_state();
		if (!animstate)
			return;

		c_basecombatweapon* weapon = g::weapon();

		c_basecombatweapon* world_weapon = nullptr;
		if (weapon)
			world_weapon = weapon->get_weapon_world_model();

		update_viewmodel();

		g::setup_bones = false;

		force_update(g::local(), true, true, g::cmd_angle);

		if (!i::client_state->choked_commands) {
			g::local()->parse_layer(layers);
			g::local()->parse_pose(poses);
			g::abs_rotation = animstate->abs_yaw;

			bones.prepare_data(g::local(), g::matrix, setup_mask);
			bones.angles = vector3d(0.f, g::abs_rotation, 0.f);

			bones.setup();

			auto orig = g::local()->get_render_origin();
			for (auto& i : g::matrix) {
				i.mat[0][3] -= orig.x;
				i.mat[1][3] -= orig.y;
				i.mat[2][3] -= orig.z;
			}
		}

		// remove body swaying
		// it happenes cuz we're strafing but we look that we're moving forward
		layers[12].weight = 0.f;

		g::update_local = false;
	}

	void start(int stage) {
		if (i::client_state->delta_tick == -1)
			return;

		if (!g::in_game) {
			g::update_local = true;
			return;
		}

		if (stage != frame_render_start)
			return;

		if (!g::local() || !g::local()->is_alive())
			return;

		update_render_data();
	}
}