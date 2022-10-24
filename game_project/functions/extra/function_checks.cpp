#include "function_checks.h"

#include "../../base/sdk.h"
#include "../../base/global_variables.h"

#include "../../base/ingame objects/c_usercmd.h"
#include "../../base/ingame objects/c_animstate.h"
#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

namespace misc_checks
{
	bool is_able_to_shoot(int shift_ticks, bool revolver)
	{
		if (!g::weapon())
			return false;

		auto weapon_info = g::weapon()->get_weapon_info();
		if (!weapon_info)
			return false;

		short idx = g::weapon()->definition_index();

		if (g::weapon()->is_grenade())
			return false;

		if (g::local()->flags() & 0x40)
			return false;

		if (g::local()->is_defusing())
			return false;

		if (weapon_info->weapon_type >= 1 && weapon_info->weapon_type <= 6 && g::weapon()->current_ammo() < 1)
			return false;

		float cur_time = math::ticks_to_time(g::tick_base - shift_ticks);

		if (g::local()->next_attack() > cur_time)
			return false;

		if (g::weapon()->next_primary_attack() > cur_time)
			return false;

		if (!revolver)
			return true;

		if (idx != weapon_revolver)
			return true;

		return g::weapon()->post_pone_fire_ready_time() >= cur_time;
	}

	bool is_firing(int shift_ticks)
	{
		c_basecombatweapon* weapon = g::weapon();
		if (!weapon)
			return false;

		float cur_time = math::ticks_to_time(g::tick_base - shift_ticks);

		bool attack_twice = (g::cmd->buttons & in_attack) || (g::cmd->buttons & in_attack2);

		if (weapon->is_grenade())
			return !weapon->pin_pulled() && weapon->throw_time() > 0.f && weapon->throw_time() < cur_time;

		if (weapon->is_knife())
			return attack_twice && is_able_to_shoot(shift_ticks);

		if (weapon->definition_index() == weapon_revolver)
			return (g::cmd->buttons & in_attack) && is_able_to_shoot(shift_ticks, true);

		return (g::cmd->buttons & in_attack) && is_able_to_shoot(shift_ticks);
	}
}