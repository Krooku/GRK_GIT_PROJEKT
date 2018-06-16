//#pragma GCC diagnostic ignored "-Wwrite-strings"
#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <ext.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

#include "entity_manager.h"
#include "fish.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SIZE 1

GLuint CubemapTexture;

GLuint programColor;
GLuint programTexture;
GLuint programTextureProc;
GLuint programSkybox;

GLuint programLightRay;

GLuint g_Texture;
GLuint textureGround;

Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;
obj::Model orka;
obj::Model shark;
obj::Model groundModel;

float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-20, 0, 0);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;
glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));

EntityManager* manager;

bool change = false;

float cubemapVertices[] = {
	// positions
	-SIZE,  SIZE, -SIZE,
	-SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,

	-SIZE, -SIZE,  SIZE,
	-SIZE, -SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE,  SIZE,
	-SIZE, -SIZE,  SIZE,

	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,

	-SIZE, -SIZE,  SIZE,
	-SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE, -SIZE,  SIZE,
	-SIZE, -SIZE,  SIZE,

	-SIZE,  SIZE, -SIZE,
	SIZE,  SIZE, -SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	-SIZE,  SIZE,  SIZE,
	-SIZE,  SIZE, -SIZE,

	-SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE,  SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE,  SIZE,
	SIZE, -SIZE,  SIZE
};

GLuint SkyboxVertexBuffer, SkyboxVertexAttributes;

glm::mat4 createCameraMatrix()
{
	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0, 1, 0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

void renderSkybox()
{
	glUseProgram(programSkybox);
	glm::mat4 view = glm::mat4(glm::mat3(cameraMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "view"), 1, GL_FALSE, (float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "perspective"), 1, GL_FALSE, (float*)&perspectiveMatrix);

	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(SkyboxVertexAttributes);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	glUseProgram(0);
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	//test
	GLfloat move = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.f * 2.f * 3.14159f * .75f;  // 3/4 of a wave cycle per second
	glUniform1f(glGetUniformLocation(program, "offset"), move);

	//test
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	glUniform1f(glGetUniformLocation(program, "time"), time);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint texture)
{
	GLuint program = programTexture;

	glUseProgram(program);

	Core::SetActiveTexture(texture, "texture", program, 0);

	//test
	GLfloat move = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.f * 2.f * 3.14159f * .75f; // 3/4 of a wave cycle per second
	glUniform1f(glGetUniformLocation(program, "offset"), move);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectProceduralTexture(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color) {
	GLuint program = programTextureProc;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.5f;
	switch (key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'q': cameraPos += glm::cross(cameraDir, glm::vec3(0, 0, 1)) * moveSpeed; break;
	case 'e': cameraPos -= glm::cross(cameraDir, glm::vec3(0, 0, 1)) * moveSpeed; break;

	}

}

glm::mat4 createOrbit(float m_f, float sunDistance) {
	glm::mat4 translate;
	glm::mat4 rotate;

	rotate[0][0] = cos(m_f);
	rotate[2][2] = cos(m_f);
	rotate[0][2] = sin(m_f);
	rotate[2][0] = -sin(m_f);

	translate[3][0] = 0;
	translate[3][1] = 0;
	translate[3][2] = -sunDistance;

	return rotate * translate;
}

void drawEarth(float m_f) {
	glm::mat4 modelMatrix = createOrbit(1.f, 26.f);
	drawObjectTexture(&sphereModel, modelMatrix, g_Texture);
}

void renderScene()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float m_f = time;

	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.129f, 0.191f, 0.199f, 1.0f);

	// Macierz statku "przyczepia" go do kamery. Warto przeanalizowac te linijke i zrozumiec jak to dziala.
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.25f + glm::vec3(0.5, -0.25f, 0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));
	drawObjectColor(&shipModel, shipModelMatrix, glm::vec3(0.6f));


	//glm::mat4 orcaModelMatrix = glm::translate(glm::vec3(0.f, 0.f, -20.f));
	//drawObjectColor(&orka, orcaModelMatrix, glm::vec3(0.6f));

	//glm::mat4 sharkModelMatrix = glm::translate(glm::vec3(10.f, 0.f, -20.f) );
	//drawObjectColor(&shark, sharkModelMatrix, glm::vec3(0.6f));
	manager->Update();

	manager->Render(cameraMatrix, perspectiveMatrix);
	

	drawEarth(time);

	drawObjectTexture(&groundModel, glm::translate(glm::vec3(0, -2, 0)) * glm::scale(glm::vec3(1.2f)), textureGround);

	renderSkybox();

	glUseProgram(programLightRay);

	Core::SetActiveTexture(g_Texture, "texture", programLightRay, 0);

	GLfloat move = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.f * 2.f * 3.14159f * .75f;  // 1/4 of a wave cycle per second
	glUniform1f(glGetUniformLocation(programLightRay, "offset"), move);

	glUniform1f(glGetUniformLocation(programLightRay, "iTime"), 1.f / 30.f);
	glUniform2f(glGetUniformLocation(programLightRay, "iResolution"), 110.f, 110.f);
	glUniform3f(glGetUniformLocation(programLightRay, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glColor4f(0, 0, 0, 128);
	glRectf(-1.0f, 1.0f, 1.f, -1.f);
	//glRotatef(45.f,0.f, 1.f, 0.f);
	glDisable(GL_BLEND);
	glUseProgram(0);

	glutSwapBuffers();
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programTextureProc = shaderLoader.CreateProgram("shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/skybox.vert", "shaders/skybox.frag");

	programLightRay = shaderLoader.CreateProgram("shaders/light_ray.vert", "shaders/light_ray.frag");

	g_Texture = Core::LoadTexture("textures/earth.png");
	textureGround = Core::LoadTexture("textures/groundtex.png");

	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	std::cout << sphereModel.faces.size() << " " << sphereModel.normal.size() << " " << sphereModel.texCoord.size() << " " << sphereModel.vertex.size() << std::endl;

	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	std::cout << shipModel.faces.size() << " " << shipModel.normal.size() << " " << shipModel.texCoord.size() << " " << shipModel.vertex.size() << std::endl;

	orka = obj::loadModelFromFile("models/orca.obj");
	std::cout << orka.faces.size() << " " << orka.normal.size() << " " << orka.texCoord.size() << " " << orka.vertex.size() << std::endl;

	shark = obj::loadModelFromFile("models/shark.obj");
	std::cout << shark.faces.size() << " " << shark.normal.size() << " " << shark.texCoord.size() << " " << shark.vertex.size() << std::endl;

	groundModel = obj::loadModelFromFile("models/ground2.obj");

	manager = new EntityManager();

	for(int i = 0; i < 2; i++)
		manager->Add("shark", new Fish(&shark, &programColor, glm::translate(glm::vec3(0.f, 0.f, 10.f * (float)i)), glm::vec3(0.6f, 0.6f, 0.6f)));

	//load Cubemap texture
	std::vector<std::string> faces;
	faces.push_back("textures/skybox4/right.jpg");
	faces.push_back("textures/skybox4/left.jpg");
	faces.push_back("textures/skybox4/top.jpg");
	faces.push_back("textures/skybox4/bottom.jpg");
	faces.push_back("textures/skybox4/back.jpg");
	faces.push_back("textures/skybox4/front.jpg");


	CubemapTexture = loadCubemap(faces);

	//Skybox settings
	glGenBuffers(1, &SkyboxVertexBuffer);
	glGenVertexArrays(1, &SkyboxVertexAttributes);
	glBindVertexArray(SkyboxVertexAttributes);
	glBindBuffer(GL_ARRAY_BUFFER, SkyboxVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), cubemapVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(programSkybox);

	delete manager;
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Podwodny swiat");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);
	glutMainLoop();
	shutdown();

	return 0;
}
