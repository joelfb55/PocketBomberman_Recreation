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
	int remainingTime = 0;
	int lifes = 0;
	bool heart = 0;
	int bombs = 0;
	bool remote = 0;
	int size = 0;
};

#endif