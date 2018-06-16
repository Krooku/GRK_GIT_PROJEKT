#pragma once
#include "entity.h"

class Fish : public Entity
{
public:
	Fish(obj::Model* model, GLuint* program, glm::mat4 modelMatrix, glm::vec3 color);
	Fish(obj::Model* model, GLuint* program, glm::mat4 modelMatrix, GLuint* texture);
	~Fish();

	bool Update();
	void Render(glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix);

private:
	obj::Model* model;
	GLuint* programShader;

	glm::mat4 modelMatrix;
	glm::mat4 translate;
	glm::mat4 rotate;

	GLuint* modelTexture;

	//to_do
	float x, y, z;
	float speed;
	float anglex, angley, anglez;

	bool texture;
};