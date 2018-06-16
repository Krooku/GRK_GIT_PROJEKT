#include "entity.h"

Entity::Entity()
{
	this->active = true;
	
}
Entity::~Entity()
{
}


void Entity::Destory()
{
	this->setActive();
}

bool Entity::getActive()
{
	return this->active;
}
void Entity::setActive()
{
	this->active = !this->active;
}

