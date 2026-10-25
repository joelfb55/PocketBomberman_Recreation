#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Game.h"
#include "Cursor.h"
#include "SoundManager.h"


using namespace std;

#define POINTINGPLAY glm::vec3(0.025, -0.323, 0.0)
#define POINTINGINST glm::vec3(0.025, -0.47, 0.0)
#define POINTINGCREDITS glm::vec3(0.025, -0.617, 0.0)
#define SCALE glm::vec3(0.05, 0.05, 0.05)

Cursor::~Cursor() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Cursor::init() {

	canChangeDown = true;
	canChangeUp = true;

	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/simple.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/simple.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	simple.init();
	simple.addShader(vShader);
	simple.addShader(fShader);
	simple.link();
	if (!simple.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << simple.log() << endl << endl;
	}
	simple.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();


	vector<float> vertices;

	// First triangle
	vertices.push_back(-1.f); vertices.push_back(-1.f);
	vertices.push_back(0.f); vertices.push_back(0.f);
	vertices.push_back(-1.f); vertices.push_back(1.f);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = simple.bindVertexAttribute("position", 2, 2 * sizeof(float), 0);

	p = Pointing::PLAY;
	canChangeUp = canChangeDown = true;
	pointingPlay = glm::translate(glm::mat4(1.0), POINTINGPLAY);
	pointingPlay = glm::scale(pointingPlay, SCALE);
	pointingInst = glm::translate(glm::mat4(1.0), POINTINGINST);
	pointingInst = glm::scale(pointingInst, SCALE);
	pointingCredits = glm::translate(glm::mat4(1.0), POINTINGCREDITS);
	pointingCredits = glm::scale(pointingCredits, SCALE);


}

void Cursor::update(int deltaTime) {
	if (canChangeUp && Game::instance().getKey(GLFW_KEY_UP)) {
		if (p == Pointing::CREDITS) p = Pointing::INST;
		else if (p == Pointing::INST) p = Pointing::PLAY;
		else if (p == Pointing::PLAY) p = Pointing::CREDITS;
		canChangeUp = false;
		SoundManager::instance().playSFX("sound/effects/Cursor.wav");

	}
	if (canChangeDown && Game::instance().getKey(GLFW_KEY_DOWN)) {
		if (p == Pointing::PLAY) p = Pointing::INST;
		else if (p == Pointing::INST) p = Pointing::CREDITS;
		else if (p == Pointing::CREDITS) p = Pointing::PLAY;
		canChangeDown = false;
		SoundManager::instance().playSFX("sound/effects/Cursor.wav");

	}
	if (!canChangeUp && !Game::instance().getKey(GLFW_KEY_UP)) {

		canChangeUp = true;
	}
	if (!canChangeDown && !Game::instance().getKey(GLFW_KEY_DOWN)) {
		canChangeDown = true;

	}

}

void Cursor::render() {
	simple.use();
	simple.setUniformMatrix4f("projection", glm::mat4(1.0f));
	simple.setUniform4f("color", 0.0f, 0.0f, 0.0f, 1.0f);
	if(p == Pointing::PLAY) simple.setUniformMatrix4f("modelview", pointingPlay);
	else if (p == Pointing::INST) simple.setUniformMatrix4f("modelview", pointingInst);
	else if (p == Pointing::CREDITS) simple.setUniformMatrix4f("modelview", pointingCredits);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

Pointing Cursor::getPointing() { return p; }