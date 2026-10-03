#pragma once
#pragma once
#ifndef _FLOATING_ENEMY_INCLUDE
#define _FLOATING_ENEMY_INCLUDE

#include "Enemy.h"

class FloatingEnemy : public Enemy {
public:
    // FUNCIONES DE OBJETO
    void init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) override;

protected:
    // FUNCIONES DE JUEGO
    void updateLogic(int deltaTime, TileMap* map) override;

private:
    // Estados
    bool movingRight = true;

    // Atributos
    float baseY = 0.0f;       // Y original alrededor de la cual oscila
    float sinAngle = 0.0f;    // Ángulo acumulado en radianes

};

#endif