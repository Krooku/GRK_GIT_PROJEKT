#include "fish.h"

Fish::Fish(obj::Model* model, GLuint* program, glm::mat4 modelMatrix, glm::vec3 color)
{
	this->texture = false;

	this->model = model;
	this->programShader = program;
	this->modelMatrix = modelMatrix;
}
Fish::Fish(obj::Model* model, GLuint* program, glm::mat4 modelMatrix, GLuint* texture)
{
	this->texture = true;

	this->model = model;
	this->programShader = program;
	this->modelMatrix = modelMatrix;
	this->modelTexture = texture;
}

Fish::~Fish()
{
}

bool Fish::Update()
{

	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	//glm::mat4 translate;
	glm::mat4 rotate;

	rotate[0][0] = cos(time);
	rotate[2][2] = cos(time);
	rotate[0][2] = sin(time);
	rotate[2][0] = -sin(time);

	this->translate[3][0] = 0.f;//x
	this->translate[3][1] += 0.001f;//y
	this->translate[3][2] = 10.f;// -sunDistance; //z

	this->modelMatrix = rotate * this->translate;
	return Entity::Update();
}
void Fish::Render(glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix)
{
	glUseProgram(*this->programShader);

	if (this->texture)
	{
		Core::SetActiveTexture(*this->modelTexture, "texture", *this->programShader, 0);
	}
	//test
	GLfloat move = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.f * 2.f * 3.14159f * .75f;  // 3/4 of a wave cycle per second
	glUniform1f(glGetUniformLocation(*this->programShader, "offset"), move);

	glUniform3f(glGetUniformLocation(*this->programShader, "lightDir"), 1.f, 1.f, 1.f);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * this->modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(*this->programShader, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(*this->programShader, "modelMatrix"), 1, GL_FALSE, (float*)&this->modelMatrix);

	Core::DrawModel(this->model);
	//Entity::Redner(this->model);

	glUseProgram(0);
}