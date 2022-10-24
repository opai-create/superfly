#include "setup_bones_manager.h"

#include "../../base/other/game_functions.h"

namespace bone_merge
{
	uintptr_t& get_bone_merge_ptr(c_csplayer* player)
	{
		return *(uintptr_t*)((uintptr_t)player + *patterns[pattern_t::get_bone_merge].as<uintptr_t*>());
	}

	void update_cache(uintptr_t bonemerge)
	{
		game_fn::update_merge_cache(bonemerge);
	}
}

mstudio_pose_param_desc_t* get_pose_parameter_ptr(c_studiohdr* hdr, int index)
{
	return game_fn::get_pose_parameter(hdr, index);
}

float get_pose_parameter_value(c_studiohdr* hdr, int index, float value)
{
	if (index < 0 || index > 24)
		return 0.f;

	mstudio_pose_param_desc_t* pose_parameter_ptr = get_pose_parameter_ptr(hdr, index);

	if (!pose_parameter_ptr)
		return 0.f;

	mstudio_pose_param_desc_t pose_parameter = *pose_parameter_ptr;

	if (pose_parameter.loop)
	{
		float wrap = (pose_parameter.start + pose_parameter.end) / 2.f + pose_parameter.loop / 2.f;
		float shift = pose_parameter.loop - wrap;

		value = value - pose_parameter.loop * std::floorf((value + shift) / pose_parameter.loop);
	}

	return (value - pose_parameter.start) / (pose_parameter.end - pose_parameter.start);
}

void merge_matching_pose_params(uintptr_t& bone_merge, float* poses, float* target_poses)
{
	bone_merge::update_cache(bone_merge);

	if (*(uintptr_t*)(bone_merge + 0x10) && *(uintptr_t*)(bone_merge + 0x8C))
	{
		int* index = (int*)(bone_merge + 0x20);
		for (int i = 0; i < 24; ++i)
		{
			if (*index != -1)
			{
				c_csplayer* target = *(c_csplayer**)(bone_merge + 0x4);
				c_studiohdr* hdr = target->get_studio_hdr();
				float pose_param_value = 0.f;

				if (hdr && *(studio_hdr_t**)hdr && i >= 0)
				{
					float pose = target_poses[i];
					mstudio_pose_param_desc_t* pose_param = get_pose_parameter_ptr(hdr, i);

					pose_param_value = pose * (pose_param->end - pose_param->start) + pose_param->start;
				}

				c_csplayer* second_target = *(c_csplayer**)(bone_merge);
				c_studiohdr* second_hdr = second_target->get_studio_hdr();

				poses[*index] = get_pose_parameter_value(second_hdr, *index, pose_param_value);
			}

			++index;
		}
	}
}

void c_setupbones::prepare_data(c_csplayer* player, matrix3x4_t* matrix, int mask)
{
	c_animstate* state = player->get_animation_state();

	this->animating = player;
	this->origin = player->origin();
	this->layers = player->get_animation_layer();
	this->hdr = player->get_studio_hdr();
	this->overlay_count = 13;
	this->angles = player->get_abs_angles();
	this->bones = matrix;
	this->mask = mask;
	this->curtime = player->simulation_time();
	this->build_attachments = false;
	this->use_ik_context = false;
	this->should_dispatch = true;
	this->pose_parameters = player->pose_parameter();

	c_basecombatweapon* weapon = player->get_active_weapon();
	c_basecombatweapon* world_weapon = nullptr;
	if (weapon)
		world_weapon = weapon->get_weapon_world_model();

	if (world_weapon)
		this->world_pose_parameters = world_weapon->pose_parameter();

	this->filled = true;
}

void c_setupbones::setup()
{
	alignas(16) vector3d position[256] = {};
	alignas(16) quaternion q[256] = {};

	c_ikcontext* ik_context = *(c_ikcontext**)((uintptr_t)this->animating + *patterns[ik_context_ptr].add(2).as<uintptr_t*>() + 0x4);

	if (!this->use_ik_context)
		ik_context = nullptr;

	this->hdr = this->animating->get_studio_hdr();

	uint32_t bone_computed[8];
	std::memset(bone_computed, 0, 8 * sizeof(uint32_t));

	bool sequences_available = !*(int*)(*(uintptr_t*)this->hdr + 0x150) || *(int*)((uintptr_t)this->hdr + 0x4);

	if (ik_context)
	{
		ik_context->init(this->hdr, &this->angles, &this->origin, this->curtime, i::global_vars->frame_count, this->mask);

		if (sequences_available)
			get_skeleton(position, q);

		ik_context->update_targets(position, q, this->bones, (uint8_t*)bone_computed);
		this->animating->calc_ik_locks(this->curtime);
		ik_context->solve_dependencies(position, q, this->bones, (uint8_t*)bone_computed);
	}
	else if (sequences_available)
		get_skeleton(position, q);

	matrix3x4_t transform;
	transform.angle_matrix(this->angles, this->origin);

	studio_build_matrices(this->hdr, transform, position, q, this->mask, this->bones, bone_computed);

	if (this->mask & bone_used_by_attachment && this->build_attachments)
		attachment_helper();

	this->animating->last_setup_bone_time() = this->curtime;

	this->animating->get_bone_accessor()->readable_bones |= this->mask;
	this->animating->get_bone_accessor()->writable_bones |= this->mask;

	static auto invalidate_bone_cache = patterns[pattern_t::invalidate_bone_cache].as<uint64_t>();
	static auto model_bone_counter = *(uintptr_t*)(invalidate_bone_cache + 0xA);

	this->animating->model_recent_bone_counter() = *(uint32_t*)model_bone_counter;

	// new shit from last update (21.09.2021)
	game_fn::modify_body_yaw(this->animating, this->bones, this->mask);
}

bool can_be_animated(c_csplayer* player)
{
	static int custom_player = *patterns[custom_player_ptr].add(2).as<int*>();

	if (!*(bool*)((uintptr_t)player + custom_player) || !player->get_animation_state())
		return false;

	c_basecombatweapon* weapon = player->get_active_weapon();

	if (!weapon)
		return false;

	c_csplayer* world_model = (c_csplayer*)weapon->get_weapon_world_model();

	if (!world_model || *(short*)((uintptr_t)world_model + 0x26E) == -1)
		return player == g::local();

	return true;
}

void c_setupbones::get_skeleton(vector3d* position, quaternion* q)
{
	alignas(16) vector3d new_position[256];
	alignas(16) quaternion new_q[256];

	c_ikcontext* ik_context = *(c_ikcontext**)((uintptr_t)this->animating + *patterns[ik_context_ptr].add(2).as<uintptr_t*>() + 0x4);

	if (!this->use_ik_context)
		ik_context = nullptr;

	alignas(16) char buffer[32];
	alignas(16) bone_setup_t* bone_setup = (bone_setup_t*)&buffer;

	bone_setup->hdr = this->hdr;
	bone_setup->mask = this->mask;
	bone_setup->pose_parameter = this->pose_parameters.data();
	bone_setup->pose_debugger = nullptr;

	bone_setup->init_pose(position, q, this->hdr);
	bone_setup->accumulate_pose(position, q, this->animating->sequence(), this->animating->cycle(), 1.f, this->curtime, ik_context);

	int layer[13] = {};

	for (int i = 0; i < this->overlay_count; ++i)
	{
		c_animationlayer& final_layer = this->layers[i];

		if (final_layer.weight > 0.f && final_layer.order != 15 && final_layer.order >= 0 && final_layer.order < this->overlay_count)
			layer[final_layer.order] = i;
	}

	char tmp_buffer[4208];
	c_ikcontext* world_ik = (c_ikcontext*)tmp_buffer;

	c_basecombatweapon* weapon = this->animating->get_active_weapon();

	c_csplayer* weapon_world_model = nullptr;
	if (weapon)
		weapon_world_model = (c_csplayer*)weapon->get_weapon_world_model();

	//for (int i = 0; i < 24; i++)
		//log_console("pose: %d | world val: %0.2f | val: %0.2f \n", i, this->world_pose_parameters[i], this->pose_parameters[i]);

	if (can_be_animated(this->animating) && weapon_world_model)
	{
		uintptr_t bone_merge = bone_merge::get_bone_merge_ptr(weapon_world_model);

		if (bone_merge)
		{
			merge_matching_pose_params(bone_merge, this->world_pose_parameters.data(), this->pose_parameters.data());

			c_studiohdr* world_hdr = weapon_world_model->get_studio_hdr();

			world_ik->construct();
			world_ik->init(world_hdr, &this->angles, &this->origin, this->curtime, 0, bone_used_by_bone_merge);

			alignas(16) char buffer2[32];
			alignas(16) bone_setup_t* world_setup = (bone_setup_t*)&buffer2;

			world_setup->hdr = world_hdr;
			world_setup->mask = bone_used_by_bone_merge;
			world_setup->pose_parameter = this->world_pose_parameters.data();
			world_setup->pose_debugger = nullptr;

			world_setup->init_pose(new_position, new_q, world_hdr);

			for (int i = 0; i < this->overlay_count; ++i)
			{
				c_animationlayer* layer = &this->layers[i];

				if (layer && layer->sequence > 1 && layer->weight > 0.f)
				{
					if (this->should_dispatch && this->animating == g::local())
						this->animating->update_dispatch_layer(layer, world_hdr, layer->sequence);

					if (!this->should_dispatch || layer->second_dispatch_sequence <= 0 || layer->second_dispatch_sequence >= (*(studio_hdr_t**)world_hdr)->local_seq)
						bone_setup->accumulate_pose(position, q, layer->sequence, layer->cycle, layer->weight, this->curtime, ik_context);
					else if (this->should_dispatch)
					{
						game_fn::copy_from_follow(bone_merge, position, q, bone_used_by_bone_merge, new_position, new_q);

						if (ik_context)
							game_fn::add_dependencies(ik_context,
								*(float*)((uintptr_t)this->animating + 0xA14),
								layer->sequence, layer->cycle, this->pose_parameters.data(), layer->weight);

						world_setup->accumulate_pose(new_position, new_q, layer->second_dispatch_sequence,
							layer->cycle, layer->weight, this->curtime, world_ik);

						game_fn::copy_to_follow(bone_merge, new_position, new_q, bone_used_by_bone_merge, position, q);
					}
				}
			}

			world_ik->destruct();
		}
	}
	else
	{
		for (int i = 0; i < this->overlay_count; ++i)
		{
			int layer_count = layer[i];

			if (layer_count >= 0 && layer_count < this->overlay_count)
			{
				c_animationlayer* final_layer = &this->layers[i];
				bone_setup->accumulate_pose(position, q,
					final_layer->sequence, final_layer->cycle,
					final_layer->weight, this->curtime, ik_context);
			}
		}
	}

	if (ik_context)
	{
		world_ik->construct();
		world_ik->init(this->hdr, &this->angles, &this->origin, this->curtime, 0, this->mask);
		bone_setup->calc_autoplay_sequences(position, q, this->curtime, world_ik);
		world_ik->destruct();
	}
	else
		bone_setup->calc_autoplay_sequences(position, q, this->curtime, nullptr);

	bone_setup->calc_bone_adjust(position, q, (float*)((uintptr_t)this->animating + 0xA54), this->mask);
}

void c_setupbones::studio_build_matrices(c_studiohdr* hdr, const matrix3x4_t& world_transform, vector3d* pos, quaternion* q, int mask, matrix3x4_t* out, uint32_t* bone_computed)
{
	int i = 0;
	int chain_length = 0;
	int bone = -1;
	studio_hdr_t* studio_hdr = *(studio_hdr_t**)hdr;

	if (bone < -1 || bone >= studio_hdr->bones)
		bone = 0;

	c_utlvector<int>* bone_parent = (c_utlvector<int>*)((uintptr_t)hdr + 0x44);
	c_utlvector<int>* bone_flags = (c_utlvector<int>*)((uintptr_t)hdr + 0x30);

	int chain[256];

	if (bone <= -1)
	{
		chain_length = studio_hdr->bones;

		for (i = 0; i < studio_hdr->bones; ++i)
			chain[chain_length - i - 1] = i;
	}
	else
	{
		i = bone;

		do
		{
			chain[chain_length++] = i;
			i = bone_parent->m_memory[i];
		} while (i != -1);
	}

	matrix3x4_t bone_matrix;

	for (int j = chain_length - 1; j >= 0; --j)
	{
		i = chain[j];

		if ((1 << (i & 0x1F)) & bone_computed[i >> 5])
			continue;

		int flag = bone_flags->m_memory[i];
		int parent = bone_parent->m_memory[i];

		if (flag & mask && q)
		{
			bone_matrix.quaternion_matrix(q[i], pos[i]);

			if (parent == -1)
				out[i] = world_transform.contact_transforms(bone_matrix);
			else
				out[i] = out[parent].contact_transforms(bone_matrix);
		}
	}
}

void c_setupbones::attachment_helper()
{
	game_fn::attachments_helper(this->animating, this->hdr);
}

void c_setupbones::fix_bones_rotations()
{
	c_studiohdr* studio_hdr = this->animating->get_studio_hdr();

	if (studio_hdr)
	{
		studio_hdr_t* hdr = *(studio_hdr_t**)studio_hdr;

		if (hdr)
		{
			mstudio_hitbox_set_t* hitbox_set = hdr->get_hitbox_set(this->animating->hitbox_set());

			for (int i = 0; i < hitbox_set->hitboxes; ++i)
			{
				mstudio_bbox_t* hitbox = hitbox_set->get_hitbox(i);

				if (!hitbox->rotation.valid())
					continue;

				matrix3x4_t hitbox_transform;
				hitbox_transform.angle_matrix(hitbox->rotation);

				this->bones[hitbox->bone] = this->bones[hitbox->bone].contact_transforms(hitbox_transform);
			}
		}
	}
}