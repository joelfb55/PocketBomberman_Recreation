#pragma once
#ifndef _ENEMY_INCLUDE
#define _ENEMY_INCLUDE

#include <cmath>
#include <glm/glm.hpp>
#include "Sprite.h"
#include "TileMap.h"

enum class EnemyState { ALIVE, DYING, DEAD };

class Enemy {
public:
    // FUNCIONES DE OBJETO
    virtual ~Enemy();
    virtual void init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) = 0;

    // FUNCIONES DE JUEGO
    void update(int deltaTime, TileMap* map, glm::vec2 player_pos);
    void render();

    // FUNCIONES AUXILIARES
    bool checkCollision(const glm::vec2& playerPos, const glm::ivec2& playerSize) const;
    glm::vec2 getPosition() const;
    std::vector<glm::ivec2> getOccupiedTilePositions(int tileSize) const;

    virtual void kill();
    bool isAlive() const;
    bool isReallyAlive() const;

protected:
    virtual void updateLogic(int deltaTime, TileMap* map, glm::vec2 player_pos) = 0;

protected:
    // Render
    Sprite* sprite = nullptr;
    Texture spritesheet;
    glm::ivec2 tileDispl;

    // Atributos
    glm::vec2 position;
    EnemyState state = EnemyState::ALIVE;
    float deathTimer = 0.f;

};

#endif // _ENEMY_INCLUDE