#pragma once
#ifndef _CURSOR_INCLUDE
#define _CURSOR_INCLUDE

#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"

enum class Pointing {PLAY, INST, CREDITS};

class Cursor
{
public:
	~Cursor();

	void init();
	void update(int deltaTime);
	void render();

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	ShaderProgram simple;

	Pointing p;
	bool canChangeUp, canChangeDown;

	glm::mat4 pointingPlay, pointingInst, pointingCredits;
};

#endif
