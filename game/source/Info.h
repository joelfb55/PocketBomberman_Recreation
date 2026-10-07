#pragma once
#ifndef _INFO_INCLUDE
#define _INFO_INCLUDE

#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


class Info
{
public:
	~Info();

	void init(ShaderProgram& program);
	void update(int deltaTime);
	void render();

private:
	void configureTexture(Texture* t);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	ShaderProgram* texProgram;
	Texture background, timerLogo, lifesLogo, bombsLogo, sizeLogo;
};

#endif