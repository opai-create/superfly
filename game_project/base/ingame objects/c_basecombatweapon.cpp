#include "c_basecombatweapon.h"

weapon_info_t* c_basecombatweapon::get_weapon_info()
{
	using fn = weapon_info_t*(__thiscall*)(void*);
	return memory::vfunc<fn>(this, 461)(this);
}

c_basecombatweapon* c_basecombatweapon::get_weapon_world_model()
{
	c_basecombatweapon* weapon_world_model = (c_basecombatweapon*)i::entity_list->get_entity_handle(this->weapon_world_model());
	return weapon_world_model;
}
