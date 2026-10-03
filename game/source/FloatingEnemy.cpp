#include "FloatingEnemy.h"

#define ENEMY_SIZE 16.f

#define ENEMY_MOVE_SPEED 0.6f
#define ENEMY_WAVE_AMPLITUDE 16.f   // Amplitud de oscilación vertical (px)
#define ENEMY_WAVE_SPEED 0.003f     // Velocidad angular (rad/ms)

// FUNCIONES DE OBJETO
void FloatingEnemy::init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) {
    position = pos;
    baseY = pos.y;
    sinAngle = 0.0f;

    tileDispl = tileMapDispl;
    state = EnemyState::ALIVE;

    spritesheet.loadFromFile("images/FloatingEnemy.png", TEXTURE_PIXEL_FORMAT_RGBA);
    spritesheet.setWrapS(GL_CLAMP_TO_EDGE);
    spritesheet.setWrapT(GL_CLAMP_TO_EDGE);
    spritesheet.setMinFilter(GL_NEAREST);
    spritesheet.setMagFilter(GL_NEAREST);
    sprite = Sprite::createSprite(glm::ivec2(ENEMY_SIZE, ENEMY_SIZE), glm::vec2(0.25f, 0.25f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(2);

    // Caminar
    sprite->setAnimationSpeed(0, 6);
    sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    sprite->addKeyframe(0, glm::vec2(0.25, 0.f));

    // Muerte
    sprite->setAnimationSpeed(1, 8);
    sprite->addKeyframe(1, glm::vec2(0.5f, 0.f));
    sprite->addKeyframe(1, glm::vec2(0.0f, 0.25f));
    sprite->addKeyframe(1, glm::vec2(0.25f, 0.25f));

    sprite->changeAnimation(0);
    sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
}

// FUNCIONES DE JUEGO
void FloatingEnemy::updateLogic(int deltaTime, TileMap* map) {
    constexpr float PI = 3.14159265359f;
    constexpr float TWO_PI = 6.28318530718f;


    float oldY = position.y;
    // Avanzar  angulo sin
    sinAngle += ENEMY_WAVE_SPEED * static_cast<float>(deltaTime);
    if (sinAngle >= TWO_PI) {
        sinAngle -= TWO_PI;
    }
    position.y = baseY + ENEMY_WAVE_AMPLITUDE * std::sin(sinAngle);

    // Colisiones Eje Y
    int correctedY = static_cast<int>(position.y);
    if (position.y < oldY) {
        if (map->collisionMoveUp(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE), &correctedY)) {
            position.y = static_cast<float>(correctedY);
            sinAngle = 3.0f * PI - sinAngle; // Invierte la fase hacia abajo
        }
    }
    else if (position.y > oldY) {
        if (map->collisionMoveDown(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE), &correctedY)) {
            position.y = static_cast<float>(correctedY);
            sinAngle = PI - sinAngle;       // Invierte la fase hacia arriba
        }
    }

    // Normalizar sinAngle
    while (sinAngle < 0.0f) sinAngle += TWO_PI;
    while (sinAngle >= TWO_PI) sinAngle -= TWO_PI;

    // Colisiones Horizontales Eje X
    if (movingRight) {
        position.x += ENEMY_MOVE_SPEED;
        if (map->collisionMoveRight(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE))) {
            position.x -= ENEMY_MOVE_SPEED;
            movingRight = false;
        }
    }
    else {
        position.x -= ENEMY_MOVE_SPEED;
        if (map->collisionMoveLeft(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE))) {
            position.x += ENEMY_MOVE_SPEED;
            movingRight = true;
        }
    }

    sprite->setFlipped(movingRight);
    sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
    sprite->update(deltaTime);
}