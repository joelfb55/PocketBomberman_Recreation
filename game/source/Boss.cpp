#include "Boss.h"

#define BOSS_SIZE 32.f

#define BOSS_VELOCITY 1.f
#define ATTACK1_TIME 1000.f
#define MIN 3000
#define MAX 5000


// Funciones de objecto
Boss::~Boss() { 
    if (sprite) delete sprite;
}

// Inicialicación del boss
void Boss::init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) {
    position = pos;
    std::srand(std::time(nullptr));

    tileDispl = tileMapDispl;
    state = BossState::MOVING;
    direction = glm::vec2(2, 2);
    time_next_attack = rand() % (MAX - MIN) + MIN;

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
    sprite->addKeyframe(0, glm::vec2(0.25, 0.f));

    // Attack
    sprite->setAnimationSpeed(1, 8);
    sprite->addKeyframe(1, glm::vec2(0.5f, 0.f));

    sprite->changeAnimation(0);
    sprite->setFlipped(1);
    sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
}

void Boss::update(int deltaTime, TileMap* map, glm::vec2 player_pos) {
    if (state == BossState::MOVING) {
        glm::vec2 oldPos = position;
        position += BOSS_VELOCITY * direction / 4.f;
        int dummy = static_cast<int>(position.y);
        if (direction.x > 0 && map->collisionMoveRight(glm::ivec2(position.x, oldPos.y), glm::vec2(BOSS_SIZE, BOSS_SIZE))) {
            direction = glm::vec2(-direction.x, direction.y);
            position = oldPos + BOSS_VELOCITY * direction / 4.f;
            sprite->setFlipped(false);
        }
        if (direction.x < 0 && map->collisionMoveLeft(glm::ivec2(position.x, oldPos.y), glm::vec2(BOSS_SIZE, BOSS_SIZE))) {
            direction = glm::vec2(-direction.x, direction.y);
            position = oldPos + BOSS_VELOCITY * direction / 4.f;
            sprite->setFlipped(true);
        }
        if (direction.y > 0 && map->collisionMoveDown(glm::ivec2(oldPos.x, position.y), glm::vec2(BOSS_SIZE, BOSS_SIZE), &dummy)) {
            direction = glm::vec2(direction.x, -direction.y);
            position = oldPos + BOSS_VELOCITY * direction / 4.f;
        }
        if (direction.y < 0 && map->collisionMoveUp(glm::ivec2(oldPos.x, position.y), glm::vec2(BOSS_SIZE, BOSS_SIZE), &dummy)) {
            direction = glm::vec2(direction.x, -direction.y);
            position = oldPos + BOSS_VELOCITY * direction / 4.f;
        }
        if (time_next_attack < 0) {
            state = BossState::ATTACK1;
            sprite->setFlipped(false);
            time_next_attack = rand() % (MAX - MIN) + MIN;
            attacking_time = 0;
        }
        sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
        
    }
    else if (state == BossState::ATTACK1) {
        sprite->changeAnimation(1);
        if (attacking_time > ATTACK1_TIME) {
            state = BossState::MOVING;
            sprite->changeAnimation(0);
        }
        attacking_time += deltaTime;
    }
    time_next_attack -= deltaTime;
    sprite->update(deltaTime);
}

void Boss::render() {
    if (sprite && state != BossState::DEAD) {
        sprite->render();
    }
}