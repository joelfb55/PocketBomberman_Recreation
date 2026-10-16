#include "Projectile.h"

Projectile::~Projectile() { if (sprite) delete sprite; }

void Projectile::init(glm::vec2 initialPos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram, ProjectileType type) {
    position = initialPos;
    tileDispl = tileMapDispl;
    state = ProjectileState::INACTIVE;

    spritesheet.loadFromFile("images/Projectile.png", TEXTURE_PIXEL_FORMAT_RGBA);
    spritesheet.setWrapS(GL_CLAMP_TO_EDGE);
    spritesheet.setWrapT(GL_CLAMP_TO_EDGE);
    spritesheet.setMinFilter(GL_NEAREST);
    spritesheet.setMagFilter(GL_NEAREST);
    sprite = Sprite::createSprite(glm::ivec2(16.f, 16.f), glm::vec2(0.25f, 0.5f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(1);

    if (type == ProjectileType::SIMPLE) {
        velocity = 1.f;
        projectileSize = 8;
        sprite->setAnimationSpeed(0, 6);
        sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
        sprite->addKeyframe(0, glm::vec2(0.25f, 0.f));
    }
    else {
        velocity = 0.5f;
        projectileSize = 12;
        sprite->setAnimationSpeed(0, 6);
        sprite->addKeyframe(0, glm::vec2(0.f, 0.5f));
        sprite->addKeyframe(0, glm::vec2(0.25f, 0.5f));
        sprite->addKeyframe(0, glm::vec2(0.5f, 0.5f));
    }

    sprite->changeAnimation(0);
    sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
}

void Projectile::update(int deltaTime, TileMap* map, glm::vec2 player_pos) {
    if (state == ProjectileState::ACTIVE) {
        position = position + velocity * direction / glm::length(direction);
        int dummy = position.y;
        glm::vec2 fix = glm::vec2((16.f - projectileSize) / 2.f, (16.f - projectileSize) / 2.f);
        if (map->collisionBorderMoveDown(position + fix, glm::vec2(projectileSize, projectileSize), &dummy) ||
            map->collisionBorderMoveUp(position + fix, glm::vec2(projectileSize, projectileSize), &dummy) ||
            map->collisionBorderMoveRight(position + fix, glm::vec2(projectileSize, projectileSize)) ||
            map->collisionBorderMoveLeft(position + fix, glm::vec2(projectileSize, projectileSize))) {
            state = ProjectileState::INACTIVE;
        }
        sprite->update(deltaTime);
        sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
    }
}

ProjectileState Projectile::getState() { return state; }

void Projectile::activate(glm::vec2 pos, glm::vec2 dir) {
    state = ProjectileState::ACTIVE;
    sprite->changeAnimation(0);
    position = pos;
    direction = dir;
}

void Projectile::render() {
    if (state == ProjectileState::ACTIVE) {
        sprite->render();
    }
}