#pragma once
#include "../../base/hooks/hooks.h"

#include "../../base/interfaces/entity_list.h"
#include "../../base/ingame objects/c_csplayer.h"

#include <vector>

enum listener_ent_t
{
	ent_weapon = 0,
	ent_grenade,
	ent_player,
	ent_fog,
	ent_tonemap,
};

bool is_bomb( int class_id );

struct entity_info_t
{
	c_baseentity* entity;
	int idx;
	int class_id;

	entity_info_t() : entity(nullptr), idx(-1), class_id(-1) { }

	entity_info_t(c_baseentity* ent)
	{
		idx = ent->index();
		entity = ent;
		class_id = ent->get_client_class()->class_id;
	}
};

class c_listener_entity : public c_entity_listener
{
private:
	std::vector< entity_info_t > entities[5] = {};
public:
	const std::vector< entity_info_t >& get_entity(int type);

	void on_entity_created(c_baseentity* entity) override;
	void on_entity_deleted(c_baseentity* entity) override;

	void init_listener();
	void remove_listener();
};

extern c_listener_entity* listener_entity;