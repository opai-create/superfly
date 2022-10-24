#include "c_baseentity.h"

c_baseentity* c_baseentity::get_move_parent()
{
	c_baseentity* move_parent = (c_baseentity*)i::entity_list->get_entity_handle(this->move_parent());
	return move_parent;
}