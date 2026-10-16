#include "Boss.h"

#define BOSS_SIZE 32.f

#define BOSS_VELOCITY 2.f
#define ATTACK1_TIME 1000.f
#define MIN 3000
#define MAX 5000
#define NUMBER_ATTACKS 3


// Funciones de objecto
Boss::~Boss() { 
    if (sprite) delete sprite;
}

// Inicialicación del boss
void Boss::init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) {
    position = pos;
    heavy_attack_finished = true;
    simple_attack_finished = true;
    std::srand(std::time(nullptr));

    tileDispl = tileMapDispl;
    state = BossState::MOVING;
    direction = glm::vec2(2, 2);
    time_next_attack = rand() % (MAX - MIN) + MIN;
    next_attack = 0;

    spritesheet.loadFromFile("images/Boss.png", TEXTURE_PIXEL_FORMAT_RGBA);
    spritesheet.setWrapS(GL_CLAMP_TO_EDGE);
    spritesheet.setWrapT(GL_CLAMP_TO_EDGE);
    spritesheet.setMinFilter(GL_NEAREST);
    spritesheet.setMagFilter(GL_NEAREST);
    sprite = Sprite::createSprite(glm::ivec2(BOSS_SIZE, BOSS_SIZE), glm::vec2(0.25f, 1.f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(2);

    // Caminar
    sprite->setAnimationSpeed(0, 6);
    sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    sprite->addKeyframe(0, glm::vec2(0.25f, 0.f));

    // Attack
    sprite->setAnimationSpeed(1, 8);
    sprite->addKeyframe(1, glm::vec2(0.5f, 0.f));

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
                rightMinion->activate(position + glm::vec2(8.f, 8.f), true);
                leftMinion->activate(position + glm::vec2(8.f, 8.f), false);
                time_next_attack = rand() % (MAX - MIN) + MIN;
                next_attack = rand() % NUMBER_ATTACKS;
                attacking_time = 0;
            }
            else next_attack = (next_attack + 1) % 3;
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
    leftMinion->update(deltaTime, map, player_pos);
    rightMinion->update(deltaTime, map, player_pos);
    heavy->update(deltaTime, map, player_pos);
    simpleNW->update(deltaTime, map, player_pos);
    simpleWS->update(deltaTime, map, player_pos);
    simpleSE->update(deltaTime, map, player_pos);
    simpleEN->update(deltaTime, map, player_pos);
    time_next_attack -= deltaTime;
    sprite->update(deltaTime);
}

void Boss::render() {
    if (sprite && state != BossState::DEAD) {
        sprite->render();
    }
    heavy->render();
    leftMinion->render();
    rightMinion->render();
    simpleNW->render();
    simpleWS->render();
    simpleSE->render();
    simpleEN->render();
}