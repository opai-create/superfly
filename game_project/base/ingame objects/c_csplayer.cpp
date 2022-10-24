#include "c_csplayer.h"
#include "c_basecombatweapon.h"

#include "../other/game_functions.h"

vector3d c_csplayer::bone_position(int bone_index)
{
	matrix3x4_t* matrix = this->get_bone_accessor()->bones;
	return matrix[bone_index].get_origin();
}

vector3d c_csplayer::hitbox_position(int hitbox, matrix3x4_t* matrix)
{
	if (!matrix)
		matrix = this->get_bone_cache().base();

	auto hdr = i::model_info->get_studio_model(this->get_model());

	if (!hdr)
		return vector3d();

	auto hitbox_set = hdr->get_hitbox_set(this->hitbox_set());

	if (!hitbox_set)
		return vector3d();

	auto hdr_hitbox = hitbox_set->get_hitbox(hitbox);

	if (!hdr_hitbox)
		return vector3d();

	vector3d min, max;

	math::vector_transform(hdr_hitbox->bbmin, matrix[hdr_hitbox->bone], min);
	math::vector_transform(hdr_hitbox->bbmax, matrix[hdr_hitbox->bone], max);

	return (min + max) * 0.5f;
}

c_basecombatweapon* c_csplayer::get_active_weapon()
{
	c_basecombatweapon* active_weapon = (c_basecombatweapon*)i::entity_list->get_entity_handle(this->active_weapon());
	return active_weapon;
}

c_studiohdr* c_csplayer::get_studio_hdr()
{
	return *(c_studiohdr**)((uintptr_t)this + *patterns[studio_hdr_ptr].add(2).as<uintptr_t*>() + 4);
}

void c_csplayer::run_pre_think()
{
	if (game_fn::physics_run_think(this, 0))
	{
		using pre_think_fn = void(__thiscall*)(void*);
		memory::vfunc<pre_think_fn>(this, 317)(this);
	}
}

void c_csplayer::run_think()
{
	// GetNextThinkTick()
	int& think_tick = *(int*)((uintptr_t)this + 0x3F);
	if (think_tick <= 0 || think_tick > this->tick_base())
		return;

	think_tick = -1;

	// unk func inside RunThink()
	game_fn::think(this, 0);

	using think_fn = void(__thiscall*)();
	memory::vfunc<think_fn>(this, 138)();
}

vector3d c_csplayer::eye_pos()
{
	vector3d eye_pos = this->get_abs_origin() + this->view_offset();

	auto state = this->get_animation_state();
	if (!state)
		return eye_pos;

	int bone = game_fn::lookup_bone(this, "head_0");

	bool fix_eye_pos = state->landing || state->anim_duck_amount != 0.f || !(this->flags() & fl_onground);

	if (bone != -1 && fix_eye_pos)
	{
		vector3d head_pos = bone_position(bone);

		auto head_height = head_pos.z + 1.7f;
		if (eye_pos.z > head_height)
		{
			auto tmp = (std::fabsf(eye_pos.z - head_height) - 4.0f) * 0.16666667f;
			if (tmp >= 0.0f)
				tmp = std::fminf(tmp, 1.0f);
			eye_pos.z = ((head_height - eye_pos.z)
				* (((tmp * tmp) * 3.0f) - (((tmp * tmp) * 2.0f) * tmp)))
				+ eye_pos.z;
		}
	}

	return eye_pos;
}

unsigned int find_in_datamap(datamap_t* pMap, const char* name)
{
	while (pMap)
	{
		for (int i = 0; i < pMap->dataNumFields; i++)
		{
			if (pMap->dataDesc[i].fieldName == nullptr)
				continue;

			if (!strcmp(name, pMap->dataDesc[i].fieldName))
				return pMap->dataDesc[i].fieldOffset;

			if (pMap->dataDesc[i].fieldType == FIELD_EMBEDDED)
			{
				if (pMap->dataDesc[i].td)
				{
					unsigned int offset = {};

					if ((offset = find_in_datamap(pMap->dataDesc[i].td, name)) != 0)
						return offset;
				}
			}
		}
		pMap = pMap->baseMap;
	}

	return 0;
}