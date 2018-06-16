#pragma once

#include <unordered_map>
#include <vector>

#include "entity.h"

class EntityManager
{
public:
	EntityManager();

	void Add(std::string name, Entity* entity);
	Entity* Get(std::string name);

	bool Update();
	void Render(glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix);

	~EntityManager();
private:
	std::unordered_map<std::string, Entity*> entities;
};