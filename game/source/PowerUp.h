#pragma once
#ifndef _POWERUP_INCLUDE
#define _POWERUP_INCLUDE

#include "Player.h"
#include "Sprite.h"

class PowerUp {
public:
    // FUNCIONES DE OBJETO
    virtual ~PowerUp();

    // FUNCIONES DE JUEGO
    void update(int deltaTime);
    void render();

    // FUNCIONES AUXILIARES
    bool checkCollision(const glm::vec2& playerPos) const;

    virtual void applyEffect(Player* player) = 0;

protected:
    // Render
    Sprite* sprite = nullptr;
    Texture spritesheet;

    // Atributos
    glm::vec2 position;
};

#endif // _POWERUP_INCLUDE