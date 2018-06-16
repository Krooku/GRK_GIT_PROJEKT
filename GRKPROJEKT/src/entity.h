#pragma once
#include <glew.h>                                                
#include <freeglut.h>                                                 
#include <glm.hpp>

#include "Render_Utils.h"
#include "Texture.h"

class Entity
{
private:

	bool active;


public:
	Entity();
	~Entity();


	virtual bool Update()
	{
		return true;
	}
	virtual void Render(glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix)
	{
	}
	void Destory();

	bool getActive();
	void setActive();
};