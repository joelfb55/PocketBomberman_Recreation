#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Info.h"



Info::~Info() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Info::configureTexture(Texture* t) {
	t->setWrapS(GL_CLAMP_TO_EDGE);
	t->setWrapT(GL_CLAMP_TO_EDGE);
	t->setMinFilter(GL_NEAREST);
	t->setMagFilter(GL_NEAREST);
}

void Info::init(ShaderProgram& program) {

	vector<float> vertices;
	texProgram = &program;

	background.loadFromFile("images/InfoBack.png", TEXTURE_PIXEL_FORMAT_RGBA);
	configureTexture(&background);
	timerLogo.loadFromFile("images/timerLogo.png", TEXTURE_PIXEL_FORMAT_RGBA);
	configureTexture(&timerLogo);
	lifesLogo.loadFromFile("images/lifesLogo.png", TEXTURE_PIXEL_FORMAT_RGBA);
	configureTexture(&lifesLogo);
	bombsLogo.loadFromFile("images/bombsLogo.png", TEXTURE_PIXEL_FORMAT_RGBA);
	configureTexture(&bombsLogo);
	sizeLogo.loadFromFile("images/sizeLogo.png", TEXTURE_PIXEL_FORMAT_RGBA);
	configureTexture(&sizeLogo);

	// First triangle
	vertices.push_back(0.f); vertices.push_back(0.f);
	vertices.push_back(0.f); vertices.push_back(1.f);
	vertices.push_back(1.f); vertices.push_back(0.f);
	vertices.push_back(1.f); vertices.push_back(1.f);
	vertices.push_back(1.f); vertices.push_back(1.f);
	vertices.push_back(1.f); vertices.push_back(0.f);

	// Second triangle
	vertices.push_back(0.f); vertices.push_back(0.f);
	vertices.push_back(0.f); vertices.push_back(1.f);
	vertices.push_back(1); vertices.push_back(1);
	vertices.push_back(1.f); vertices.push_back(0.f);
	vertices.push_back(0.f); vertices.push_back(1.f);
	vertices.push_back(0.f); vertices.push_back(0.f);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
}

void Info::update(int deltaTime){

}

void Info::render() {
	glEnable(GL_TEXTURE_2D);
	glBindVertexArray(vao);
	glm::mat4 projection = glm::ortho(0.0, 64.0, 0.0, 48.0);
	texProgram->setUniformMatrix4f("projection", projection);
	texProgram->setUniform2f("texCoordDispl", 0.f, 0.f);

	// Background
	background.use();
	glm::mat4 modelview = glm::mat4(1.0f);
	//modelview = glm::translate(modelview, glm::vec3(0.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(64.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Timer
	timerLogo.use();
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(3.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Vides
	lifesLogo.use();
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(20.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Bombes
	bombsLogo.use();
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(35.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Mida
	sizeLogo.use();
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(50.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisable(GL_TEXTURE_2D);
}