#include "../tools/memory.h"

#include "../other/game_functions.h"

#include "c_animstate.h"

void c_animstate::create()
{
	game_fn::create_animstate(this);
}

void c_animstate::reset()
{
	game_fn::reset_animstate(this);
}

void c_animstate::update(const vector3d& angle, bool force)
{
	game_fn::update_animstate(this, NULL, NULL, angle.y, angle.x, force);
}

float c_animstate::get_max_body_rotation()
{
	float speed_walk = max(0.f, min(this->speed_as_portion_of_walk_top_speed, 1.f));
	float speed_duck = max(0.f, min(this->speed_as_portion_of_crouch_top_speed, 1.f));

	float modifier = ((this->walk_run_transition * -0.30000001f) - 0.19999999f) * speed_walk + 1.f;

	if (this->anim_duck_amount > 0.0f)
		modifier += ((this->anim_duck_amount * speed_duck) * (0.5f - modifier));

	return this->aim_yaw_max * modifier;
}
