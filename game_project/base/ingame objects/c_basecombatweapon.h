#pragma once
#include "../sdk.h"

#include "../tools/math.h"
#include "../tools/memory.h"
#include "../tools/netvar_parser.h"

#include "../global_variables.h"

#include "c_baseentity.h"

enum weapon_type
{
	weapontype_knife = 0,
	weapontype_pistol,
	weapontype_submachinegun,
	weapontype_rifle,
	weapontype_shotgun,
	weapontype_sniper_rifle,
	weapontype_machinegun,
	weapontype_c4,
	weapontype_placeholder,
	weapontype_grenade,
	weapontype_unknown
};

enum item_defenition_index
{
	weapon_invalid = -1,
	weapon_deagle = 1,
	weapon_elite,
	weapon_fiveseven,
	weapon_glock,
	weapon_ak47 = 7,
	weapon_aug,
	weapon_awp,
	weapon_famas,
	weapon_g3sg1,
	weapon_galilar = 13,
	weapon_m249,
	weapon_m4a1 = 16,
	weapon_mac10,
	weapon_p90 = 19,
	weapon_mp5 = 23,
	weapon_ump45,
	weapon_xm1014,
	weapon_bizon,
	weapon_mag7,
	weapon_negev,
	weapon_sawedoff,
	weapon_tec9,
	weapon_taser,
	weapon_hkp2000,
	weapon_mp7,
	weapon_mp9,
	weapon_nova,
	weapon_p250,
	weapon_shield,
	weapon_scar20,
	weapon_sg556,
	weapon_ssg08,
	weapon_knifegg,
	weapon_knife,
	weapon_flashbang,
	weapon_hegrenade,
	weapon_smokegrenade,
	weapon_molotov,
	weapon_decoy,
	weapon_incgrenade,
	weapon_c4,
	weapon_healthshot = 57,
	weapon_knife_t = 59,
	weapon_m4a1_silencer,
	weapon_usp_silencer,
	weapon_cz75a = 63,
	weapon_revolver,
	weapon_tagrenade = 68,
	weapon_fists,
	weapon_breachcharge,
	weapon_tablet = 72,
	weapon_melee = 74,
	weapon_axe,
	weapon_hammer,
	weapon_spanner = 78,
	weapon_knife_ghost = 80,
	weapon_firebomb,
	weapon_diversion,
	weapon_frag_grenade,
	weapon_snowball,
	weapon_bumpmine,
	weapon_bayonet = 500,
	weapon_knife_flip = 505,
	weapon_knife_gut,
	weapon_knife_karambit,
	weapon_knife_m9_bayonet,
	weapon_knife_tactical,
	weapon_knife_falchion = 512,
	weapon_knife_survival_bowie = 514,
	weapon_knife_butterfly,
	weapon_knife_push,
	weapon_knife_ursus = 519,
	weapon_knife_gypsy_jackknife,
	weapon_knife_stiletto = 522,
	weapon_knife_widowmaker,
	glove_studded_bloodhound = 5027,
	glove_t_side = 5028,
	glove_ct_side = 5029,
	glove_sporty = 5030,
	glove_slick = 5031,
	glove_leather_wrap = 5032,
	glove_motorcycle = 5033,
	glove_specialist = 5034,
	glove_hydra = 5035
};

struct weapon_info_t
{
	char vtable[4];
	char console_name[4]; //0x0004
	char pad_0008[12]; //0x0008
	int max_ammo_1; //0x0014
	char pad_0018[12]; //0x0018
	int max_ammo_2; //0x0024
	char pad_0028[4]; //0x0028
	char world_model[4]; //0x002C
	char view_model[4]; //0x0030
	char droped_model[4]; //0x0034
	char pad_0038[80]; //0x0038
	char* hud_name; //0x0088
	char weapon_name[4]; //0x008C
	char pad_0090[56]; //0x0090
	int weapon_type; //0x00C8
	char pad_00CC[4]; //0x00CC
	int weapon_price; //0x00D0
	int kill_award; //0x00D4
	char pad_00D8[24]; //0x00D8
	int dmg; //0x00F0
	float crosshair_delta_distance; //0x00F4
	float armor_ratio; //0x00F8
	int bullets; //0x00FC
	float penetration; //0x0100
	float flinch_velocity_modifier_large; //0x0104
	float flinch_velocity_modifier_small; //0x0108
	float range; //0x010C
	float range_modifier; //0x0110
	float throw_velocity; // 0x0110
	char pad_0x010C[12]; // 0x011C
	bool silencer; // 0x011D
	char pad_0x0119[3];	// 0x0120
	char* silencer_model; // 0x0124
	int	crosshair_min_dist;	// 0x0128
	float crosshair_delta_dist;// 0x0130 - iTeam?
	float max_speed; //0x0134
	float max_speed_alt; //0x0138
	char pad_013C[36]; //0x013C
	float inaccuracy_jump; //0x0160
	float inaccuracy_jump_alt; //0x0164
	float inaccuracy_land; //0x0168
	float inaccuracy_land_alt; //0x016C
	float inaccuracy_ladder; //0x0170
	float inaccuracy_ladder_alt; //0x0174
	float inaccuracy_stand; //0x0178
	float inaccuracy_stand_alt; //0x017C
	float inaccuracy_crouch; //0x0180
	float inaccuracy_crouch_alt; //0x0184
};

class c_basecombatweapon : public c_baseentity
{
public:
	ADD_VAR(burst_shot_remaining, int, this, offsets::m_iBurstShotsRemaining)
	ADD_VAR(current_ammo, int, this, offsets::m_iClip1)
	ADD_VAR(zoom_level, int, this, offsets::m_zoomLevel)
	ADD_VAR(next_secondary_attack, float, this, offsets::m_flNextSecondaryAttack)
	ADD_VAR(next_primary_attack, float, this, offsets::m_flNextPrimaryAttack)
	ADD_VAR(next_burst_shot, float, this, offsets::m_fNextBurstShot)
	ADD_VAR(last_shot_time, float, this, offsets::m_fLastShotTime)
	ADD_VAR(throw_time, float, this, offsets::m_fThrowTime)
	ADD_VAR_REF(post_pone_fire_ready_time, float, this, offsets::m_flPostponeFireReadyTime)
	ADD_VAR_REF(accuracy_penalty, float, this, offsets::m_fAccuracyPenalty)
	ADD_VAR_REF(recoil_index, float, this, offsets::m_flRecoilIndex)
	ADD_VAR(pin_pulled, bool, this, offsets::m_bPinPulled)
	ADD_VAR(definition_index, short, this, offsets::m_iItemDefinitionIndex)
	ADD_VAR(weapon_world_model, uint32_t, this, offsets::m_hWeaponWorldModel)

	float get_spread()
	{
		using fn = float(__thiscall*)(void*);
		return memory::vfunc<fn>(this, 453)(this);
	}

	float get_inaccuracy()
	{
		using fn = float(__thiscall*)(void*);
		return memory::vfunc<fn>(this, 483)(this);
	}

	void update_accuracy_penalty()
	{
		using fn = void(__thiscall*)(void*);
		return memory::vfunc<fn>(this, 484)(this);
	}

	int get_zoom_fov(int lvl)
	{
		using fn = int(__thiscall*)(void*, int);
		return memory::vfunc<fn>(this, 457)(this, lvl);
	}

	bool is_knife()
	{
		short idx = this->definition_index();
		return this->get_weapon_info()->weapon_type == weapontype_knife && idx != weapon_taser;
	}
	
	bool is_misc_weapon()
	{
		short idx = this->definition_index();
		int type = this->get_weapon_info()->weapon_type;
		return type == weapontype_knife
			|| type == weapontype_c4
			|| type == weapontype_placeholder
			|| type == weapontype_grenade
			|| type == weapontype_unknown
			|| idx == weapon_taser
			|| idx == weapon_healthshot;
	}

	bool is_grenade()
	{
		short idx = this->definition_index();
		return idx >= weapon_flashbang && idx <= weapon_incgrenade;
	}

	bool is_auto_sniper()
	{
		short idx = this->definition_index();

		switch (idx)
		{
		case weapon_g3sg1:
		case weapon_scar20:
			return true;
		default:
			return false;
		}
	}

	bool is_sniper()
	{
		short idx = this->definition_index();

		switch (idx)
		{
		case weapon_awp:
		case weapon_g3sg1:
		case weapon_scar20:
		case weapon_ssg08:
			return true;
		default:
			return false;
		}
	}

	weapon_info_t* get_weapon_info();
	c_basecombatweapon* get_weapon_world_model();
};