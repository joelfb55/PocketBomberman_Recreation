#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Info.h"


#define NUMBER_SIZE 13
#define MIDA_FONT_X 8.f
#define MIDA_FONT_Y 4.f

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

	font.loadFromFile("images/info.png", TEXTURE_PIXEL_FORMAT_RGBA);
	configureTexture(&font);

	float texture_x = 1 / MIDA_FONT_X;
	float texture_y = 1 / MIDA_FONT_Y;

	// First triangle
	vertices.push_back(0.f); vertices.push_back(0.f);
	vertices.push_back(0.f); vertices.push_back(texture_y);
	vertices.push_back(1.f); vertices.push_back(0.f);
	vertices.push_back(texture_x); vertices.push_back(texture_y);
	vertices.push_back(1.f); vertices.push_back(1.f);
	vertices.push_back(texture_x); vertices.push_back(0.f);

	// Second triangle
	vertices.push_back(0.f); vertices.push_back(0.f);
	vertices.push_back(0.f); vertices.push_back(texture_y);
	vertices.push_back(1); vertices.push_back(1);
	vertices.push_back(texture_x); vertices.push_back(0.f);
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

void Info::update(int deltaTime, Player* player, int level_time, int e){
	remainingTime = level_time;
	lifes = player->getVidas();
	heart = player->isHeartActive();
	bombs = player->getMaxBombs();
	remote = player->isRemoteActive();
	size = player->getExplosionRadius();
	enemies = e;
}

// Se puedae mejorar, caclculando a partir de i con modulos y tal.
glm::vec2 Info::num2coords(int i) {
	switch (i) {
	case 0: return glm::vec2(0 / MIDA_FONT_X, 0 / MIDA_FONT_Y);
	case 1: return glm::vec2(1 / MIDA_FONT_X, 0 / MIDA_FONT_Y);
	case 2: return glm::vec2(2 / MIDA_FONT_X, 0 / MIDA_FONT_Y);
	case 3: return glm::vec2(3 / MIDA_FONT_X, 0 / MIDA_FONT_Y);
	case 4: return glm::vec2(0 / MIDA_FONT_X, 1 / MIDA_FONT_Y);
	case 5: return glm::vec2(1 / MIDA_FONT_X, 1 / MIDA_FONT_Y);
	case 6: return glm::vec2(2 / MIDA_FONT_X, 1 / MIDA_FONT_Y);
	case 7: return glm::vec2(3 / MIDA_FONT_X, 1 / MIDA_FONT_Y);
	case 8: return glm::vec2(0 / MIDA_FONT_X, 2 / MIDA_FONT_Y);
	case 9: return glm::vec2(1 / MIDA_FONT_X, 2 / MIDA_FONT_Y);
	}
}

void Info::render() {
	glEnable(GL_TEXTURE_2D);
	glBindVertexArray(vao);
	glm::mat4 projection = glm::ortho(0.0, 64.0, 0.0, 48.0);
	texProgram->setUniformMatrix4f("projection", projection);
	font.use();

	glm::vec2 numTexCoords;

	/* Background */
	texProgram->setUniform2f("texCoordDispl", 2 / MIDA_FONT_X, 2 / MIDA_FONT_Y);
	glm::mat4 modelview = glm::mat4(1.0f);
	modelview = glm::scale(modelview, glm::vec3(64.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/* Timer */
	texProgram->setUniform2f("texCoordDispl", 3 / MIDA_FONT_X, 2 / MIDA_FONT_Y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(3.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	// Primer número
	int min = remainingTime / 60000;
	numTexCoords = num2coords(min);
	texProgram->setUniform2f("texCoordDispl", numTexCoords.x, numTexCoords.y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(6.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	
	// Segon número
	int ms = remainingTime % 60000;
	int sec1 = ms / 10000;
	int sec2 = ms / 1000 - sec1*10;
	numTexCoords = num2coords(sec1);
	texProgram->setUniform2f("texCoordDispl", numTexCoords.x, numTexCoords.y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(11.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	numTexCoords = num2coords(sec2);
	texProgram->setUniform2f("texCoordDispl", numTexCoords.x, numTexCoords.y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(14.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	/* Vides */

	if (heart) {
		texProgram->setUniform2f("texCoordDispl", 3 / MIDA_FONT_X, 3 / MIDA_FONT_Y);
		modelview = glm::mat4(1.0f);
		modelview = glm::translate(modelview, glm::vec3(22.0, 0.0, 0.0));
		modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
		texProgram->setUniformMatrix4f("modelview", modelview);
		glDrawArrays(GL_TRIANGLES, 0, 6); // Logo 2
	}

	texProgram->setUniform2f("texCoordDispl", 0 / MIDA_FONT_X, 3 / MIDA_FONT_Y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(25.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6); // Logo 2

	numTexCoords = num2coords(lifes);
	texProgram->setUniform2f("texCoordDispl", numTexCoords.x, numTexCoords.y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(28.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6); // Num

	/* Remaining enemies */
	texProgram->setUniform2f("texCoordDispl", 4 / MIDA_FONT_X, 0 / MIDA_FONT_Y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(35.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6); // Logo 2

	numTexCoords = num2coords(enemies);
	texProgram->setUniform2f("texCoordDispl", numTexCoords.x, numTexCoords.y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(38.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6); // Num


	/* Bombes */
	texProgram->setUniform2f("texCoordDispl", 1 / MIDA_FONT_X, 3 / MIDA_FONT_Y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(45.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6); // Logo

	numTexCoords = num2coords(bombs);
	texProgram->setUniform2f("texCoordDispl", numTexCoords.x, numTexCoords.y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(48.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6); // Num

	/* Mida */
	texProgram->setUniform2f("texCoordDispl", 2 / MIDA_FONT_X, 3 / MIDA_FONT_Y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(55.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6); // Logo

	numTexCoords = num2coords(size);
	texProgram->setUniform2f("texCoordDispl", numTexCoords.x, numTexCoords.y);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(58.0, 0.0, 0.0));
	modelview = glm::scale(modelview, glm::vec3(3.0, 3.0, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6); // Num

	// Dos punts
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(9.75, 0.5, 0.0));
	modelview = glm::scale(modelview, glm::vec3(0.70, 0.70, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	texProgram->setUniform4f("color", 0.f, 0.f, 0.f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, 6); // Down
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(9.75, 1.8, 0.0));
	modelview = glm::scale(modelview, glm::vec3(0.70, 0.70, 1.0));
	texProgram->setUniformMatrix4f("modelview", modelview);
	glDrawArrays(GL_TRIANGLES, 0, 6); // Up
	texProgram->setUniform4f("color", 1.f, 1.f, 1.f, 1.0f);

	glDisable(GL_TEXTURE_2D);
}