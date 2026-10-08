#pragma once
#ifndef _INFO_INCLUDE
#define _INFO_INCLUDE

#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include "Sprite.h"
#include "Player.h"


class Info
{
public:
	~Info();

	void init(ShaderProgram& program);
	void update(int deltaTime, Player* player, int level_time);
	void render();

private:
	void configureTexture(Texture* t);
	glm::vec2 num2coords(int i);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	ShaderProgram* texProgram;
	Texture font;

	// Game info
	int remainingTime;
	int lifes;
	bool heart;
	int bombs;
	bool remote;
	int size;
};

#endif