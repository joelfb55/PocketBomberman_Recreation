#pragma once
#ifndef _BOSS_INCLUDE
#define _BOSS_INCLUDE

#include <cmath>
#include <glm/glm.hpp>
#include <ctime>
#include <random>
#include "Sprite.h"
#include "TileMap.h"
#include "Projectile.h"
#include "Minion.h"
#include "Explosion.h"

enum class BossState { MOVING, ATTACK1, ATTACK2, ATTACK3, DYING, DEAD };

class Boss {
public:
    // FUNCIONES DE OBJETO
    ~Boss();
    void init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram);

    // FUNCIONES DE JUEGO
    void update(int deltaTime, TileMap* map, glm::vec2 player_pos);
    void render();

    // FUNCIONES AUXILIARES
    bool touchesExplosion(std::vector<glm::ivec2> affectedTiles, int tileSize);
    void getsDamage();
    bool checkCollision(const glm::vec2& playerPos, const glm::ivec2& playerSize) const;

    void kill();

    // getters
    Minion* getLeftMinion();
    Minion* getRightMinion();

    // Consultoras
    bool isDead();

private:
    // Render
    Sprite* sprite;
    std::vector<Sprite*> particles;
    std::vector<int> spawnTime;
    std::vector<int> lifeTime;
    Texture spritesheet;
    Texture particle_spritesheet;
    glm::ivec2 tileDispl;
    ShaderProgram* texProgram;

    // Atributos
    int vida;
    glm::vec2 position;
    BossState state;
    glm::vec2 direction;
    int time_next_attack;
    int next_attack;
    int attacking_time;
    bool simple_attack_finished, heavy_attack_finished, minion_attack_finished;
    int deathTimer = 0;

    // Immunity
    bool immunity = false;
    int immunity_time = 0;
    bool visible = true;
    int visible_time = 0, invisible_time = 0;

    // Projectiles
    Projectile *simpleNW, *simpleWS, *simpleSE, *simpleEN;
    Projectile* heavy;

    // Minions
    Minion* leftMinion, * rightMinion;
    int minion_living_time = 0;

};

#endif 