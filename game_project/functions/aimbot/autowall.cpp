#include "autowall.h"

#include "../../base/sdk.h"
#include "../../base/global_variables.h"

#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

bool c_autowall::is_breakable_entity(c_baseentity* e)
{
	if (!e || !e->index())
		return false;

	auto take_damage = *(uintptr_t*)(patterns[is_breakable].as<uintptr_t>() + 0x26);
	auto backup = *(uint8_t*)((uintptr_t)e + take_damage);

	auto client_class = e->get_client_class();
	auto network_name = client_class->network_name;

	if (!strcmp(network_name, "CBreakableSurface"))
		*(uint8_t*)((uintptr_t)e + take_damage) = damage_yes;
	else if (!strcmp(network_name, "CBaseDoor") || !strcmp(network_name, "CDynamicProp"))
		*(uint8_t*)((uintptr_t)e + take_damage) = damage_no;

	auto result = game_fn::is_breakable(e);

	*(uint8_t*)((uintptr_t)e + take_damage) = backup;

	return result;
}

void c_autowall::scale_damage(c_csplayer* e, int group, weapon_info_t* weaponData, float& currentDamage)
{
	if (!e->is_player())
		return;

	auto IsArmored = [&]()->bool
	{
		switch (group)
		{
		case hitgroup_head:
			return e->has_helmet();
		case hitgroup_generic:
		case hitgroup_chest:
		case hitgroup_stomach:
		case hitgroup_leftarm:
		case hitgroup_rightarm:
			return true;
		default:
			return false;
		}
	};

	auto HasHeavyArmor = e->has_heavy_armor();

	switch (group)
	{
	case hitgroup_head:
		currentDamage *= HasHeavyArmor ? 2.0f : 4.0f;
		break;
	case hitgroup_stomach:
		currentDamage *= 1.25f;
		break;
	case hitgroup_leftleg:
	case hitgroup_rightleg:
		currentDamage *= 0.75f;
		break;
	}

	auto ArmorValue = e->armor();

	if (ArmorValue > 0 && IsArmored())
	{
		auto armorBonusRatio = 0.5f;
		auto armorRatio = weaponData->armor_ratio / 2.0f;
		auto bonusValue = 1.0f;

		if (HasHeavyArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if (HasHeavyArmor)
			NewDamage *= 0.85f;

		if ((currentDamage - currentDamage * armorRatio) * bonusValue * armorBonusRatio > ArmorValue)
			NewDamage = currentDamage - ArmorValue / armorBonusRatio;

		currentDamage = NewDamage;
	}
}

bool c_autowall::trace_to_exit(c_game_trace& enterTrace, c_game_trace& exitTrace, vector3d startPosition, const vector3d& direction)
{
	auto start = vector3d();
	auto end = vector3d();

	auto currentDistance = 0.0f;
	auto firstContents = 0;

	while (currentDistance <= 90.0f)
	{
		currentDistance += 4.0f;
		start = startPosition + direction * currentDistance;

		if (!firstContents)
			firstContents = i::engine_trace->get_point_contents(start, mask_shot_hull | contents_hitbox, nullptr);

		auto pointContents = i::engine_trace->get_point_contents(start, mask_shot_hull | contents_hitbox, nullptr);

		if (!(pointContents & mask_shot_hull) || pointContents & contents_hitbox && pointContents != firstContents)
		{
			end = start - direction * 4.0f;
			i::engine_trace->trace_ray(ray_t(start, end), mask_shot_hull | contents_hitbox, nullptr, &exitTrace);

			//i::debug_overlay->add_line_overlay(start, end, 255, 255, 255, true, 0.1f);


			if (exitTrace.start_solid && exitTrace.surface.flags & surf_hitbox)
			{
				uint32_t filter[4] = { *patterns[trace_filter].add(0x3D).as<uint32_t*>(),
							(uint32_t)exitTrace.entity, 0, 0 };
				//i::debug_overlay->add_line_overlay(start, startPosition, 0, 0, 255, true, 0.1f);

				i::engine_trace->trace_ray(ray_t(start, startPosition), mask_shot_hull | contents_hitbox,
					(c_trace_filter*)filter, &exitTrace);

				if (exitTrace.hit() && !exitTrace.start_solid)
					return true;

				continue;
			}

			if (exitTrace.hit() && !exitTrace.start_solid)
			{
				if (is_breakable_entity(enterTrace.entity) && is_breakable_entity(exitTrace.entity))
					return true;

				if (enterTrace.surface.flags & surf_nodraw 
					|| !(exitTrace.surface.flags & surf_nodraw) 
					&& exitTrace.plane.normal.dot(direction) <= 1.0f)
					return true;

				continue;
			}
		}
	}

	return false;
}

bool c_autowall::handle_bullet_penetration(weapon_info_t* weaponData, c_game_trace& enterTrace,
	vector3d& eyePosition, const vector3d& direction, int& possibleHitsRemaining,
	float& currentDamage, float penetrationPower,
	float ff_damage_reduction_bullets, float ff_damage_bullet_penetration)
{
	c_game_trace exitTrace;

	auto enterSurfaceData = i::phys_surface_props->get_surface_data(enterTrace.surface.surface_props);
	auto enterMaterial = enterSurfaceData->game.material;

	auto isSolidSurf = (enterTrace.contents >> 3) & contents_solid;
	auto isLightSurf = (enterTrace.surface.flags >> 7) & surf_light;

	if (!weaponData->penetration 
		|| !trace_to_exit(enterTrace, exitTrace, enterTrace.end, direction) 
		&& !(i::engine_trace->get_point_contents(enterTrace.end, mask_shot_hull, nullptr) & mask_shot_hull))
		return false;

	auto combinedPenetrationModifier = 0.0f;
	auto finalDamageModifier = 0.0f;

	auto exitSurfaceData = i::phys_surface_props->get_surface_data(exitTrace.surface.surface_props);

	if (enterMaterial == char_tex_grate || enterMaterial == char_tex_glass)
	{
		combinedPenetrationModifier = 3.0f;
		finalDamageModifier = 0.05f;
	}
	else if (isSolidSurf || isLightSurf)
	{
		combinedPenetrationModifier = 1.0f;
		finalDamageModifier = 0.16f;
	}
	else if (enterMaterial == char_tex_flesh && ((c_csplayer*)enterTrace.entity)->team() == g::local()->team() && !ff_damage_reduction_bullets)
	{
		if (!ff_damage_bullet_penetration) //-V550
			return false;

		combinedPenetrationModifier = ff_damage_bullet_penetration;
		finalDamageModifier = 0.16f;
	}
	else
	{
		combinedPenetrationModifier = (enterSurfaceData->game.penetration_modifier + exitSurfaceData->game.penetration_modifier) / 2.0f;
		finalDamageModifier = 0.16f;
	}

	auto exitMaterial = exitSurfaceData->game.material;

	if (enterMaterial == exitMaterial)
	{
		if (exitMaterial == char_tex_cardboard || exitMaterial == char_tex_wood)
			combinedPenetrationModifier = 3.0f;
		else if (exitMaterial == char_tex_plastic)
			combinedPenetrationModifier = 2.0f;
	}

	auto thickness = (exitTrace.end - enterTrace.end).length_sqr();
	auto modifier = fmax(1.0f / combinedPenetrationModifier, 0.0f);

	//i::debug_overlay->add_line_overlay(enterTrace.end, exitTrace.end, 255, 0, 0, true, 0.1f);

	auto lostDamage = fmax((modifier * thickness / 24.0f) + currentDamage * finalDamageModifier + fmax(3.75f / penetrationPower, 0.0f) * 3.0f * modifier, 0.0f);

	if (lostDamage > currentDamage)
		return false;

	if (lostDamage > 0.0f)
		currentDamage -= lostDamage;

	if (currentDamage < 1.0f)
		return false;

	eyePosition = exitTrace.end;
	--possibleHitsRemaining;

	return true;
}

void c_autowall::clip_trace_to_player(const vector3d& start, const vector3d& end, uint32_t mask, c_game_trace* tr, c_csplayer* player)
{
	vector3d pos, to, dir, on_ray;
	float len, range_along, range;
	ray_t ray;
	c_game_trace new_trace;

	// reference: https://github.com/alliedmodders/hl2sdk/blob/3957adff10fe20d38a62fa8c018340bf2618742b/game/shared/util_shared.h#L381

	// set some local vars.
	pos = player->get_abs_origin() + ((player->mins() + player->maxs()) * 0.5f);
	to = pos - start;
	dir = start - end;
	len = dir.length(false);
	range_along = dir.dot(to);

	// off start point.
	if (range_along < 0.f)
		range = -(to).length(false);

	// off end point.
	else if (range_along > len)
		range = -(pos - end).length(false);

	// within ray bounds.
	else
	{
		on_ray = start + (dir * range_along);
		range = (pos - on_ray).length(false);
	}

	if (range <= 60.f)
	{
		i::engine_trace->clip_ray_to_entity(ray_t(start, end), mask, player, &new_trace);
		if (tr->fraction > new_trace.fraction)
			*tr = new_trace;
	}
}

bool c_autowall::fire_bullet(c_basecombatweapon* pWeapon, vector3d& direction, bool& visible, float& currentDamage, vector3d& point, int& hitbox,
	c_baseentity* e, float length, const vector3d& pos, int& hits, bool ignore_)
{
	if (!pWeapon)
		return false;

	auto weaponData = pWeapon->get_weapon_info();

	if (!weaponData)
		return false;

	c_game_trace enterTrace;
	uint32_t filter_[4] = { *patterns[trace_filter].add(0x3D).as<uint32_t*>(),
							(uint32_t)g::local(), 0, 0 };

	currentDamage = weaponData->dmg;

	auto eyePosition = pos;
	auto penetrationDistance = 3000.0f;
	auto penetrationPower = weaponData->penetration;
	hits = 4;

	float maxRange = std::min(weaponData->range, (eyePosition - point).length(false));
	auto end = eyePosition + direction * maxRange;

	while (currentDamage >= 1.0f)
	{
		i::engine_trace->trace_ray(ray_t(eyePosition, end), mask_shot_hull | contents_hitbox,
			(c_trace_filter*)filter_, &enterTrace);

		if (e)
			clip_trace_to_player(eyePosition, end + direction * 40.0f, mask_shot_hull | contents_hitbox, &enterTrace, (c_csplayer*)e);

		auto enterSurfaceData = i::phys_surface_props->get_surface_data(enterTrace.surface.surface_props);
		auto enterSurfPenetrationModifier = enterSurfaceData->game.penetration_modifier;
		auto enterMaterial = enterSurfaceData->game.material;

		const float distance_traced = (enterTrace.end - eyePosition).length(false);
		currentDamage *= std::pow(weaponData->range_modifier, (distance_traced / 500.f));

		if (enterTrace.fraction == 1.0f)
		{
			if (ignore_)
			{
				scale_damage((c_csplayer*)e, hitgroup_head, weaponData, currentDamage);
				return true;
			}
			break;
		}

		if (distance_traced > penetrationDistance && weaponData->penetration || enterSurfPenetrationModifier < 0.1f)
			break;

		auto canDoDamage = enterTrace.hitgroup != hitgroup_gear && enterTrace.hitgroup != hitgroup_generic;
		auto isPlayer = ((c_csplayer*)enterTrace.entity)->is_player();
		auto isEnemy = ((c_csplayer*)enterTrace.entity)->team() != g::local()->team();

		if (canDoDamage && isPlayer && isEnemy)
		{
			scale_damage((c_csplayer*)enterTrace.entity, enterTrace.hitgroup, weaponData, currentDamage);
			hitbox = enterTrace.hitbox;
			return true;
		}

		if (!hits)
			break;

		static auto damageReductionBullets = i::convar->find_convar("ff_damage_reduction_bullets");
		static auto damageBulletPenetration = i::convar->find_convar("ff_damage_bullet_penetration");

		if (!handle_bullet_penetration(weaponData, enterTrace, eyePosition, direction, hits, currentDamage, penetrationPower, damageReductionBullets->get_float(), damageBulletPenetration->get_float()))
			break;

		visible = false;
	}

	return false;
}

returninfo_t c_autowall::wall_penetration(const vector3d& eye_pos, vector3d& point, c_baseentity* e, bool ignore_)
{
	g::autowalling = true;

	auto tmp = point - eye_pos;

	auto dir = vector3d();
	math::angle_to_vectors(math::angle_from_vectors(eye_pos, point), dir);

	auto visible = true;
	auto damage = -1.0f;
	auto hitbox = -1;
	auto hits = -1;

	if (fire_bullet(g::weapon(), dir, visible, damage, point, hitbox, e, 0.0f, eye_pos, hits, ignore_))
	{
		g::autowalling = false;
		return returninfo_t(visible, (int)damage, hitbox, hits);
	}
	else
	{
		g::autowalling = false;
		return returninfo_t(false, -1, -1, -1);
	}
}

c_autowall* autowall = new c_autowall();