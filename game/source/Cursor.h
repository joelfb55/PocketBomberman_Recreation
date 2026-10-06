#pragma once
#ifndef _CURSOR_INCLUDE
#define _CURSOR_INCLUDE

#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


class Cursor
{
public:
	Cursor();
	~Cursor();

	void init();
	void render();

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	ShaderProgram simple;
};

#endif
