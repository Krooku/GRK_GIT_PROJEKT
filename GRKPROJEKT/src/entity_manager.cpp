#include "entity_manager.h"

EntityManager::EntityManager()
{
}

void EntityManager::Add(std::string name, Entity* entity)
{
	std::unordered_map<std::string, Entity*>::const_iterator found = this->entities.find(name);
	while (found != this->entities.end())
	{
		name += "0";
		found = this->entities.find(name);
	}
	this->entities.insert(std::make_pair(name, entity));
}

Entity* EntityManager::Get(std::string name)
{
	std::unordered_map<std::string, Entity*>::const_iterator found = this->entities.find(name);
	
	if (found == this->entities.end())
	{
		return NULL;
	}
	else
	{
		return found->second;
	}
}

bool EntityManager::Update()
{
	std::vector<std::string> toRemove;
	for (auto& iterator : this->entities)
	{
		//if (iterator.second->GroupID() > 2 || iterator.second->GroupID() == 1)
		//{
			for (auto& iterator2 : this->entities)
			{
				if (iterator.first != iterator2.first)
				{
					if (iterator.second->getActive() && iterator2.second->getActive())
					{
						//if (iterator.second->CheckCollision(iterator2.second))
						//{
						//	iterator.second->Collision(iterator2.second);
						//}
					}
				}
			}
		//}

		switch (iterator.second->getActive())
		{
		case 0:
			toRemove.push_back(iterator.first);
			break;
		default:
			if (!iterator.second->Update())
			{
				return false;
			}
			break;
		}
	}

	for (auto& iterator : toRemove)
	{
		std::unordered_map<std::string, Entity*>::const_iterator found = this->entities.find(iterator);

		if (found != this->entities.end())
		{
			delete found->second;
			this->entities.erase(iterator);
		}
	}
	toRemove.clear();

	return true;
}

void EntityManager::Render(glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix)
{
	for (auto& iterator : this->entities)
	{
		iterator.second->Render(cameraMatrix, perspectiveMatrix);
	}
}

EntityManager::~EntityManager()
{
	for (auto& iterator : this->entities)
	{
		delete iterator.second;
	}
	this->entities.clear();
}