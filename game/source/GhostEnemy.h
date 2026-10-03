#ifndef _GHOST_ENEMY_INCLUDE
#define _GHOST_ENEMY_INCLUDE


#include "Enemy.h"


class GhostEnemy : public Enemy {
public:
    // FUNCIONES DE OBJETO
    void init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) override;

protected:
    // FUNCIONES DE JUEGO
    void updateLogic(int deltaTime, TileMap* map) override;

private:
    // FUNCIONES AUXILIARES
    void applyGravity(TileMap* map);

private:
    // Atributos
    float pauseTimer = 0.0f;
    bool movingRight = true;
};

#endif