#include "Enemy.h"


#define ENEMY_SIZE 16.f
#define ENEMY_HITBOX_REDUCTION 4.f		// Indica cuantos pixeles quitamos al size para las colisiones de amplitud (x)

#define ENEMY_DEATH_ANIMATION_TIME 400.f


// FUNCIONES DE OBJETO
Enemy::~Enemy() { if (sprite) delete sprite; }

// FUNCIONES DE JUEGO
void Enemy::update(int deltaTime, TileMap* map, glm::vec2 player_pos) {
    if (state == EnemyState::DYING) {
        deathTimer -= static_cast<float>(deltaTime);
        if (sprite) sprite->update(deltaTime);
        if (deathTimer <= 0.0f) {
            state = EnemyState::DEAD;
        }
        return;
    }

    if (state == EnemyState::DEAD) return;

    // Se deriba el resto del comportamineto a las clases hijo
    updateLogic(deltaTime, map, player_pos);
}
void Enemy::render() {
    if (sprite && state != EnemyState::DEAD) {
        sprite->render();
    }
}

// FUNCIONES AUXILIARES
// Movimiento
glm::vec2 Enemy::getPosition() const { return position; }
bool Enemy::checkCollision(const glm::vec2& playerPos, const glm::ivec2& playerSize) const {
    if (state != EnemyState::ALIVE) return false;

    return (std::abs((position.x + ENEMY_SIZE / 2.f) - (playerPos.x + playerSize.x / 2.f)) < (ENEMY_SIZE - ENEMY_HITBOX_REDUCTION) &&
        std::abs((position.y + ENEMY_SIZE / 2.f) - (playerPos.y + playerSize.y / 2.f)) < (ENEMY_SIZE - ENEMY_HITBOX_REDUCTION));
}
std::vector<glm::ivec2> Enemy::getOccupiedTilePositions(int tileSize) const {
    std::vector<glm::ivec2> tiles;

    int minTileX = static_cast<int>(position.x) / tileSize;
    int maxTileX = static_cast<int>(position.x + 15.0f) / tileSize; // CUIDADO este 15 si cambiamos algo del size total
    int minTileY = static_cast<int>(position.y) / tileSize;
    int maxTileY = static_cast<int>(position.y + 15.0f) / tileSize;

    for (int ty = minTileY; ty <= maxTileY; ++ty) {
        for (int tx = minTileX; tx <= maxTileX; ++tx) {
            tiles.push_back(glm::ivec2(tx * tileSize, ty * tileSize));
        }
    }
    return tiles;
}

// Vida
bool Enemy::isAlive() const { return state != EnemyState::DEAD; }

bool Enemy::isReallyAlive() const { return state == EnemyState::ALIVE; }

void Enemy::kill() {
    if (state == EnemyState::ALIVE) {
        state = EnemyState::DYING;
        deathTimer = ENEMY_DEATH_ANIMATION_TIME;
        if (sprite) sprite->changeAnimation(1); //  1 = Muerte
    }
}