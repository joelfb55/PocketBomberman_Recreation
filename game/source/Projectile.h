#pragma once
#ifndef _ROJECTILE_INCLUDE
#define _PROJECTILE_INCLUDE

#include <glm/glm.hpp>
#include "Sprite.h"
#include "TileMap.h"

enum class ProjectileType { SIMPLE, HEAVY };
enum class ProjectileState { ACTIVE, INACTIVE };

class Projectile {
public:
    // FUNCIONES DE OBJETO
    ~Projectile();
    void init(glm::vec2 initialPos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram, ProjectileType type);

    // FUNCIONES DE JUEGO
    void update(int deltaTime, TileMap* map, glm::vec2 player_pos);
    void render();

    bool checkCollision(const glm::vec2& playerPos, const glm::ivec2& playerSize) const;


    //
    void activate(glm::vec2 pos, glm::vec2 direction);
    ProjectileState getState();

private:
    // Render
    Sprite* sprite = nullptr;
    Texture spritesheet;
    glm::ivec2 tileDispl;

    // Atributos
    glm::vec2 position;
    glm::vec2 direction;
    int projectileSize;
    float velocity;
    ProjectileState state;
};

#endif 