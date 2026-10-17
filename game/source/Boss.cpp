#include "Boss.h"

#include <iostream>

#define BOSS_SIZE 32.f
#define BOSS_HITBOX_REDUCTION 8.f

#define BOSS_VELOCITY 2.f
#define ATTACK1_TIME 1000.f
#define MIN 3000
#define MAX 5000
#define NUMBER_ATTACKS 3

#define MINION_LIVING_TIME 10000

#define IMMUNITY_TIME 1500
#define VISIBLE_TIME 5
#define INVISIBLE_TIME 5

#define DYING_TIME 2000


// Funciones de objecto
Boss::~Boss() { 
    if (sprite) delete sprite;
}

// Inicialicación del boss
void Boss::init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) {
    position = pos;
    vida = 1;
    std::srand(std::time(nullptr));

    tileDispl = tileMapDispl;
    state = BossState::MOVING;
    direction = glm::vec2(2, 2);

    // Attack variables
    time_next_attack = rand() % (MAX - MIN) + MIN;
    next_attack = 0;
    heavy_attack_finished = true;
    simple_attack_finished = true;
    attacking_time = 0;

    spritesheet.loadFromFile("images/Boss.png", TEXTURE_PIXEL_FORMAT_RGBA);
    spritesheet.setWrapS(GL_CLAMP_TO_EDGE);
    spritesheet.setWrapT(GL_CLAMP_TO_EDGE);
    spritesheet.setMinFilter(GL_NEAREST);
    spritesheet.setMagFilter(GL_NEAREST);
    sprite = Sprite::createSprite(glm::ivec2(BOSS_SIZE, BOSS_SIZE), glm::vec2(0.25f, 1.f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(3);

    // Caminar
    sprite->setAnimationSpeed(0, 6);
    sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    sprite->addKeyframe(0, glm::vec2(0.25f, 0.f));

    // Attack
    sprite->setAnimationSpeed(1, 8);
    sprite->addKeyframe(1, glm::vec2(0.5f, 0.f));

    // Die
    sprite->setAnimationSpeed(2, 8);
    sprite->addKeyframe(2, glm::vec2(0.75f, 0.f));

    sprite->changeAnimation(0);
    sprite->setFlipped(1);
    sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));

    simpleNW = new Projectile();
    simpleNW->init(position, tileMapDispl, shaderProgram, ProjectileType::SIMPLE);
    simpleWS = new Projectile();
    simpleWS->init(position, tileMapDispl, shaderProgram, ProjectileType::SIMPLE);
    simpleSE = new Projectile();
    simpleSE->init(position, tileMapDispl, shaderProgram, ProjectileType::SIMPLE);
    simpleEN = new Projectile();
    simpleEN->init(position, tileMapDispl, shaderProgram, ProjectileType::SIMPLE);

    heavy = new Projectile();
    heavy->init(position, tileMapDispl, shaderProgram, ProjectileType::HEAVY);

    leftMinion = new Minion();
    leftMinion->init(position, tileMapDispl, shaderProgram);
    rightMinion = new Minion();
    rightMinion->init(position, tileMapDispl, shaderProgram);
}

void Boss::update(int deltaTime, TileMap* map, glm::vec2 player_pos) {
    if (state == BossState::DYING) {
        if (deathTimer > 0) deathTimer -= deltaTime;
        else state = BossState::DEAD;
        return;
    }
    if (state == BossState::MOVING || state == BossState::ATTACK2) {
        glm::vec2 oldPos = position;
        position += BOSS_VELOCITY * direction / 4.f;
        int dummy = static_cast<int>(position.y);
        if (direction.x > 0 && map->collisionBorderMoveRight(glm::ivec2(position.x, oldPos.y), glm::vec2(BOSS_SIZE, BOSS_SIZE))) {
            direction = glm::vec2(-direction.x, direction.y);
            position = oldPos + BOSS_VELOCITY * direction / 4.f;
            sprite->setFlipped(false);
        }
        if (direction.x < 0 && map->collisionBorderMoveLeft(glm::ivec2(position.x, oldPos.y), glm::vec2(BOSS_SIZE, BOSS_SIZE))) {
            direction = glm::vec2(-direction.x, direction.y);
            position = oldPos + BOSS_VELOCITY * direction / 4.f;
            sprite->setFlipped(true);
        }
        if (direction.y > 0 && map->collisionBorderMoveDown(glm::ivec2(oldPos.x, position.y), glm::vec2(BOSS_SIZE, BOSS_SIZE), &dummy)) {
            direction = glm::vec2(direction.x, -direction.y);
            position = oldPos + BOSS_VELOCITY * direction / 4.f;
        }
        if (direction.y < 0 && map->collisionBorderMoveUp(glm::ivec2(oldPos.x, position.y), glm::vec2(BOSS_SIZE, BOSS_SIZE), &dummy)) {
            direction = glm::vec2(direction.x, -direction.y);
            position = oldPos + BOSS_VELOCITY * direction / 4.f;
        }
        if (time_next_attack < 0 && simple_attack_finished) {
            if (next_attack == 0 && simple_attack_finished) {
                state = BossState::ATTACK1;
                simple_attack_finished = false;
                simpleNW->activate(position + glm::vec2(8.f, 8.f), glm::vec2(-1, -1));
                simpleWS->activate(position + glm::vec2(8.f, 8.f), glm::vec2(-1, 1));
                simpleSE->activate(position + glm::vec2(8.f, 8.f), glm::vec2(1, -1));
                simpleEN->activate(position + glm::vec2(8.f, 8.f), glm::vec2(1, 1));
                time_next_attack = rand() % (MAX - MIN) + MIN;
                next_attack = rand() % NUMBER_ATTACKS;
                attacking_time = 0;
            }
            else if (next_attack == 1 && heavy_attack_finished) {
                state = BossState::ATTACK2;
                heavy_attack_finished = false;
                heavy->activate(position + glm::vec2(8.f, 8.f), player_pos - (position + glm::vec2(8.f, 8.f)));
                time_next_attack = rand() % (MAX - MIN) + MIN;
                next_attack = rand() % NUMBER_ATTACKS;
                attacking_time = 0;
            }
            else if (minion_attack_finished) {
                state = BossState::ATTACK3;
                minion_attack_finished = false;
                minion_living_time = MINION_LIVING_TIME;
                rightMinion->activate(position + glm::vec2(8.f, 8.f), true);
                leftMinion->activate(position + glm::vec2(8.f, 8.f), false);
                time_next_attack = rand() % (MAX - MIN) + MIN;
                next_attack = rand() % NUMBER_ATTACKS;
                attacking_time = 0;
            }
            else next_attack = rand() % NUMBER_ATTACKS;
        }
        sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
        
    }
    else if (state == BossState::ATTACK1 || state == BossState::ATTACK3) {
        sprite->changeAnimation(1);
        if (attacking_time > ATTACK1_TIME) {
            state = BossState::MOVING;
            sprite->changeAnimation(0);
        }
        attacking_time += deltaTime;
    }
    if (simpleNW->getState() == ProjectileState::INACTIVE && simpleWS->getState() == ProjectileState::INACTIVE &&
        simpleSE->getState() == ProjectileState::INACTIVE && simpleEN->getState() == ProjectileState::INACTIVE) {
        simple_attack_finished = true;
    }
    if (heavy->getState() == ProjectileState::INACTIVE) {
        heavy_attack_finished = true;
    }
    if (leftMinion->getState() == MinionState::INACTIVE && rightMinion->getState() == MinionState::INACTIVE) {
        minion_attack_finished = true;
    }
    if (!leftMinion->isAlive()) leftMinion->deactivate();
    if (!rightMinion->isAlive()) rightMinion->deactivate();
    // Podria fer que es pintés només si està actiu.
    leftMinion->update(deltaTime, map, player_pos);
    rightMinion->update(deltaTime, map, player_pos);
    heavy->update(deltaTime, map, player_pos);
    simpleNW->update(deltaTime, map, player_pos);
    simpleWS->update(deltaTime, map, player_pos);
    simpleSE->update(deltaTime, map, player_pos);
    simpleEN->update(deltaTime, map, player_pos);
    time_next_attack -= deltaTime;
    sprite->update(deltaTime);
    if (immunity_time > 0) {
        immunity_time -= deltaTime;
    }
    else {
        immunity = false;
    }
    if (immunity){
        if (visible) {
            if (visible_time < 0) {
                visible = false;
                invisible_time = INVISIBLE_TIME;
            }
            else visible_time -= deltaTime;
        }
        else {
            if (invisible_time < 0) {
                visible = true;
                visible_time = VISIBLE_TIME;
            }
            else invisible_time -= deltaTime;
        }
    }
    if (minion_living_time > 0) {
        minion_living_time -= deltaTime;
    }
    else {
        if (leftMinion->getState() == MinionState::ACTIVE) leftMinion->kill();
        if (rightMinion->getState() == MinionState::ACTIVE) rightMinion->kill();
    }
}

void Boss::render() {
    heavy->render();
    leftMinion->render();
    rightMinion->render();
    simpleNW->render();
    simpleWS->render();
    simpleSE->render();
    simpleEN->render();
    if (sprite && state != BossState::DEAD) {
        if (!immunity || visible) sprite->render();
    }
}

// FUNCIONES AUXILIARES
bool Boss::touchesExplosion(std::vector<glm::ivec2> affectedTiles, int tileSize) {
    for (auto it = affectedTiles.begin(); it != affectedTiles.end(); ++it) {
        int minx = (*it).x;
        int miny = (*it).y;
        int maxx = minx + tileSize;
        int maxy = miny + tileSize;

        int bossMinx = position.x;
        int bossMiny = position.y;
        int bossMaxx = bossMinx + BOSS_SIZE;
        int bossMaxy = bossMiny + BOSS_SIZE;

        if (bossMaxy > miny && maxy > bossMiny && bossMaxx > minx && maxx > bossMinx) return true;
    }
    return false;
}


void Boss::getsDamage() {
    if (!immunity) {
        vida -= 1;
        immunity = true;
        immunity_time = IMMUNITY_TIME;
        visible_time = VISIBLE_TIME;
        std::cerr << vida << std::endl;
        if (vida == 0) {
            state = BossState::DYING;
            deathTimer = DYING_TIME;
            if (leftMinion->getState() == MinionState::ACTIVE) leftMinion->kill();
            if (rightMinion->getState() == MinionState::ACTIVE) rightMinion->kill();
            sprite->changeAnimation(2);
        }
    }
}

Minion* Boss::getLeftMinion() { return leftMinion; }

Minion* Boss::getRightMinion() { return rightMinion; }

bool Boss::checkCollision(const glm::vec2& playerPos, const glm::ivec2& playerSize) const {
    if (simpleNW->getState() == ProjectileState::ACTIVE && simpleNW->checkCollision(playerPos, playerSize)) return true;
    if (simpleWS->getState() == ProjectileState::ACTIVE && simpleWS->checkCollision(playerPos, playerSize)) return true;
    if (simpleSE->getState() == ProjectileState::ACTIVE && simpleSE->checkCollision(playerPos, playerSize)) return true;
    if (simpleEN->getState() == ProjectileState::ACTIVE && simpleEN->checkCollision(playerPos, playerSize)) return true;
    if (heavy->getState() == ProjectileState::ACTIVE && heavy->checkCollision(playerPos, playerSize)) return true;
    return (std::abs((position.x + BOSS_SIZE / 2.f) - (playerPos.x + playerSize.x / 2.f)) < (BOSS_SIZE - BOSS_HITBOX_REDUCTION) &&
        std::abs((position.y + BOSS_SIZE / 2.f) - (playerPos.y + playerSize.y / 2.f)) < (BOSS_SIZE - BOSS_HITBOX_REDUCTION));
}