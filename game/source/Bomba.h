#pragma once

#include <glm/glm.hpp>
#include "Sprite.h"

class Bomba
{
public:
	// FUNCIONES DE OBJETO
	Bomba();
	~Bomba();
	void init(const glm::ivec2& tilePos, ShaderProgram& shaderProgram);

	// FUNCIONES DE JUEGO
	void update(int deltaTime);
	void render() const;

	// FUNCIONES AUXILIARES
	bool isExploded() const;
	bool isSolid() const;
	glm::ivec2 getTilePos() const;
	void updatePlayerOverlap(const glm::vec2& playerPos, const glm::ivec2& playerSize, int tileSize);

private:
	// Render
	glm::ivec2 tilePosition; // Pos
	Sprite* sprite;
	Texture spritesheet;

	// Estado
	bool playerInside = true;
	bool bExploded = false;

	// Atributos
	float timer;             // Tiempo para la explosión (mlsec)

};