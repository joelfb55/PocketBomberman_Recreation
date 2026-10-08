#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Info.h"


#define NUMBER_SIZE 13

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

	lifes = 3;

	vector<float> vertices;
	texProgram = &program;

	font.loadFromFile("images/info.png", TEXTURE_PIXEL_FORMAT_RGBA);
	configureTexture(&font);

	// First triangle
	vertices.push_back(0.f); vertices.push_back(0.f);
	vertices.push_back(0.f); vertices.push_back(0.25f);
	vertices.push_back(1.f); vertices.push_back(0.f);
	vertices.push_back(0.25f); vertices.push_back(0.25f);
	vertices.push_back(1.f); vertices.push_back(1.f);
	vertices.push_back(0.25f); vertices.push_back(0.f);

	// Second triangle
	vertices.push_back(0.f); vertices.push_back(0.f);
	vertices.push_back(0.f); vertices.push_back(0.25f);
	vertices.push_back(1); vertices.push_back(1);
	vertices.push_back(0.25f); vertices.push_back(0.f);
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
	lifes = 3;
}

glm::vec2 Info::num2coords(int i) {
	switch (i) {
	case 1: return glm::vec2(0.25, 0.0);
	case 2: return glm::vec2(0.5, 0.0);
	case 3: return glm::vec2(0.75, 0.0);
	}
}

void Info::render() {
	glEnable(GL_TEXTURE_2D);
	glBindVertexArray(vao);
	glm::mat4 projection = glm::ortho(0.0, 64.0, 0.0, 48.0);
	texProgram->setUniformMatrix4f("projection", projection);
	font.use();

	glm::vec2 numTexCoords;

	// Background
	texProgram->setUniform2f("texCoordDispl", 0.5f, 0.5f);
	glm::mat4 modelview = glm::mat4(1.0f);
	//modelview = glm::translate(modelview, glm::vec3(0.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(64.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Timer
	texProgram->setUniform2f("texCoordDispl", 0.75f, 0.5f);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(3.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Vides
	texProgram->setUniform2f("texCoordDispl", 0.f, 0.75f);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(20.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	numTexCoords = num2coords(lifes);
	texProgram->setUniform2f("texCoordDispl", numTexCoords.x, numTexCoords.y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(25.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Bombes
	texProgram->setUniform2f("texCoordDispl", 0.25f, 0.75f);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(35.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Mida
	texProgram->setUniform2f("texCoordDispl", 0.5f, 0.75f);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(50.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisable(GL_TEXTURE_2D);
}