#pragma once
#ifndef _ROJECTILE_INCLUDE
#define _PROJECTILE_INCLUDE

#include <glm/glm.hpp>
#include "Sprite.h"
#include "TileMap.h"

enum class ProjectileState { ACTIVE, INACTIVE };

class Projectile {
public:
    // FUNCIONES DE OBJETO
    ~Projectile();
    virtual void init(glm::vec2 initialPos, ShaderProgram& shaderProgram);

    // FUNCIONES DE JUEGO
    void update(int deltaTime, TileMap* map, glm::vec2 player_pos);
    void render();

private:
    // Render
    Sprite* sprite = nullptr;
    Texture spritesheet;

    // Atributos
    glm::vec2 position;
    glm::vec2 direction;
    int projectileSize;
    ProjectileState state;
};

#endif 