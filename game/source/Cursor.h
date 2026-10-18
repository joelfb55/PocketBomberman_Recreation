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

	Pointing getPointing();

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	ShaderProgram simple;

	Pointing p = Pointing::PLAY;
	bool canChangeUp = true, canChangeDown = true;

	glm::mat4 pointingPlay, pointingInst, pointingCredits;
};

#endif
