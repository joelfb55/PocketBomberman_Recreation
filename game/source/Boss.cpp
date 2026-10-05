#include "Boss.h"

#define BOSS_SIZE 32.f

#define BOSS_VELOCITY 1.f


// Funciones de objecto
Boss::~Boss() { 
    if (sprite) delete sprite;
}

// Inicialicación del boss
void Boss::init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) {
    position = pos;

    tileDispl = tileMapDispl;
    state = BossState::ALIVE;

    spritesheet.loadFromFile("images/Boss.png", TEXTURE_PIXEL_FORMAT_RGBA);
    spritesheet.setWrapS(GL_CLAMP_TO_EDGE);
    spritesheet.setWrapT(GL_CLAMP_TO_EDGE);
    spritesheet.setMinFilter(GL_NEAREST);
    spritesheet.setMagFilter(GL_NEAREST);
    sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(0.25f, 1.f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(1);

    // Caminar
    sprite->setAnimationSpeed(0, 6);
    sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    sprite->addKeyframe(0, glm::vec2(0.25, 0.f));

    // Muerte
    //sprite->setAnimationSpeed(1, 8);
    

    sprite->changeAnimation(0);
    sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
}

void Boss::update(int deltaTime, TileMap* map, glm::vec2 player_pos) {
    sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
    sprite->update(deltaTime);
}

void Boss::render() {
    if (sprite && state != BossState::DEAD) {
        sprite->render();
    }
}