#include "listener_entity.h"

/*	
	implementation from https://github.com/EternityX/DEADCELL-CSGO/blob/master/csgo/features/entity%20listener/ent_listener.cpp
	but it's very useful and optimized method
*/

bool is_grenade(const int& class_id)
{
	switch (class_id)
	{
	case (int)CBaseCSGrenadeProjectile:
	case (int)CBreachChargeProjectile:
	case (int)CBumpMineProjectile:
	case (int)CDecoyProjectile:
	case (int)CMolotovProjectile:
	case (int)CSensorGrenadeProjectile:
	case (int)CSmokeGrenadeProjectile:
	case (int)CSnowballProjectile:
	case (int)CInferno:
		return true;
		break;
	}
	return false;
}

bool is_bomb(int class_id)
{
	if (class_id == CC4 || class_id == CPlantedC4)
		return true;

	return false;
}

bool is_weapon(c_baseentity* entity, int class_id)
{
	if (is_bomb(class_id))
		return true;

	if (entity->is_weapon())
		return true;

	return false;
}

void erase_elements(int index, std::vector<entity_info_t>& info)
{
	const auto it = std::find_if(info.begin(), info.end(), [&](const entity_info_t& data)
		{
			return data.idx == index;
		});

	if (it == info.end())
		return;

	info.erase(it);
}

const std::vector< entity_info_t >& c_listener_entity::get_entity(int type)
{
	return this->entities[type];
}

void c_listener_entity::on_entity_created(c_baseentity* entity)
{
	if (!entity)
		return;

	int index = entity->index();
	c_client_class* client_class = entity->get_client_class();
	int class_id = client_class->class_id;

	if (entity->is_player())
		this->entities[ent_player].emplace_back(entity);

	if (is_weapon(entity, class_id))
		this->entities[ent_weapon].emplace_back(entity);

	if (is_grenade(class_id))
		this->entities[ent_grenade].emplace_back(entity);

	if (class_id == CFogController)
		this->entities[ent_fog].emplace_back(entity);

	if (class_id == CEnvTonemapController)
		this->entities[ent_tonemap].emplace_back(entity);
}

void c_listener_entity::on_entity_deleted(c_baseentity* entity)
{
	if (!entity)
		return;

	int index = entity->index();
	if (index < 0)
		return;

	for (int i = 0; i < 5; i++)
		erase_elements(index, this->entities[i]);
}

void c_listener_entity::init_listener()
{
	i::entity_list->add_listener_entity(this);
}

void c_listener_entity::remove_listener()
{
	i::entity_list->remove_listener_entity(this);
}

c_listener_entity* listener_entity = new c_listener_entity();