#include "PowerUp.h"


// FUNCIONES DE OBJETO
PowerUp::~PowerUp() { if (sprite) delete sprite; }

// FUNCIONES DE JUEGO
void PowerUp::update(int deltaTime) { if (sprite) sprite->update(deltaTime); }
void PowerUp::render() { if (sprite) sprite->render(); }

// FUNCIONES AUXILIARES
bool PowerUp::checkCollision(const glm::vec2& playerPos) const {
    return (abs(position.x - playerPos.x) < 16 &&
        abs(position.y - playerPos.y) < 16);
}
