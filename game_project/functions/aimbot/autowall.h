#pragma once
#include "../../base/tools/math.h"
#include "../../base/interfaces/engine_trace.h"

class c_basecombatweapon;
class c_csplayer;
class c_game_trace;
class c_trace_filter;
struct weapon_info_t;
class vector3d;

struct returninfo_t
{
	bool valid = false;

	bool visible = false;
	int damage = -1;
	int hitbox = -1;
	int hits = -1;

	returninfo_t()
	{
		this->valid = false;

		this->visible = false;
		this->damage = -1;
		this->hitbox = -1;
		this->hits = -1;
	}

	returninfo_t(bool visible, int damage, int hitbox, int hits)
	{
		this->valid = true;

		this->visible = visible;
		this->damage = damage;
		this->hitbox = hitbox;
		this->hits = hits;
	}
};


class c_autowall
{
public:
	struct FireBulletData
	{
		vector3d src;
		c_game_trace enter_trace;
		vector3d direction;
		c_trace_filter filter;
		weapon_info_t* wpn_data;
		float trace_length;
		float trace_length_remaining;
		float length_to_end;
		float current_damage;
		int penetrate_count;
	};

	bool is_breakable_entity(c_baseentity* e);
	void scale_damage(c_csplayer* e, int group, weapon_info_t* weaponData, float& currentDamage);
	bool trace_to_exit(c_game_trace& enterTrace, c_game_trace& exitTrace, vector3d startPosition, const vector3d& direction);
	bool handle_bullet_penetration(weapon_info_t* weaponData, c_game_trace& enterTrace,
		vector3d& eyePosition, const vector3d& direction, int& possibleHitsRemaining,
		float& currentDamage, float penetrationPower,
		float ff_damage_reduction_bullets, float ff_damage_bullet_penetration);
	void clip_trace_to_player(const vector3d& start, const vector3d& end, uint32_t mask, c_game_trace* tr, c_csplayer* player);
	returninfo_t wall_penetration(const vector3d& eye_pos, vector3d& point, c_baseentity* e, bool ignore_ = false);
	bool fire_bullet(c_basecombatweapon* pWeapon, vector3d& direction, bool& visible, float& currentDamage, vector3d& point, int& hitbox,
		c_baseentity* e, float length, const vector3d& pos, int& hits, bool ignore_ = false);
};

extern c_autowall* autowall;