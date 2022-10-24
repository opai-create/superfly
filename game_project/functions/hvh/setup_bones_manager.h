#pragma once
#include "../../base/tools/math.h"

#include "../../base/ingame objects/studio_hdr.h"
#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

namespace bone_merge
{
	uintptr_t& get_bone_merge_ptr(c_csplayer* player);
	void update_cache(uintptr_t bonemerge);
}

class c_setupbones
{
private:
	void get_skeleton(vector3d* position, quaternion* q);
	void studio_build_matrices(c_studiohdr* hdr,
		const matrix3x4_t& world_transform, vector3d* pos,
		quaternion* q, int mask, matrix3x4_t* out, uint32_t* bone_computed);
public:
	void attachment_helper();
	void fix_bones_rotations();
	void prepare_data(c_csplayer* player, matrix3x4_t* matrix, int mask);
	void setup();

	bool filled = false;

	bool use_ik_context = false;
	bool build_attachments = true;
	bool should_dispatch = true;

	int mask = 0;
	int overlay_count = 0;

	float curtime = 0.f;

	matrix3x4_t* bones = nullptr;
	c_studiohdr* hdr = nullptr;
	c_animationlayer* layers = nullptr;
	c_csplayer* animating = nullptr;

	vector3d origin = vector3d();
	vector3d angles = vector3d();

	std::array<float, 24> pose_parameters = {};
	std::array<float, 24> world_pose_parameters = {};
};