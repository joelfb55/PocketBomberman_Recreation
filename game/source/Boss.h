#pragma once
#ifndef _BOSS_INCLUDE
#define _BOSS_INCLUDE

#include <cmath>
#include <glm/glm.hpp>
#include <ctime>
#include <random>
#include "Sprite.h"
#include "TileMap.h"
#include "SimpleProjectile.h"

enum class BossState { MOVING, ATTACK1, DYING, DEAD };

class Boss {
public:
    // FUNCIONES DE OBJETO
    ~Boss();
    void init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram);

    // FUNCIONES DE JUEGO
    void update(int deltaTime, TileMap* map, glm::vec2 player_pos);
    void render();

    // FUNCIONES AUXILIARES
    bool checkCollision(const glm::vec2& playerPos, const glm::ivec2& playerSize) const;
    glm::vec2 getPosition() const;
    //std::vector<glm::ivec2> getOccupiedTilePositions(int tileSize) const;

    void  kill();
    bool isAlive() const;
    bool isReallyAlive() const;

private:
    // Render
    Sprite* sprite;
    Texture spritesheet;
    glm::ivec2 tileDispl;

    // Atributos
    glm::vec2 position;
    BossState state;
    glm::vec2 direction;
    int time_next_attack;
    int attacking_time;
    //float deathTimer = 0.f;

    // Projectiles

};

#endif 