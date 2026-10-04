#include "RemoteController.h"


// FUNCIONES DE OBJETO
RemoteController::RemoteController() {}

void RemoteController::init(glm::vec2 pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) {
    position = pos;

    spritesheet.loadFromFile("images/RemoteController.png", TEXTURE_PIXEL_FORMAT_RGBA);
    spritesheet.setWrapS(GL_CLAMP_TO_EDGE);
    spritesheet.setWrapT(GL_CLAMP_TO_EDGE);
    spritesheet.setMinFilter(GL_NEAREST);
    spritesheet.setMagFilter(GL_NEAREST);

    sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(1.f, 1.f), &spritesheet, &shaderProgram);

    sprite->setNumberAnimations(1);
    sprite->setAnimationSpeed(0, 1);
    sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    sprite->changeAnimation(0);

    sprite->setPosition(glm::vec2(tileMapDispl.x + position.x, tileMapDispl.y + position.y));
}

// FUNCIONES AUXILIARES
void RemoteController::applyEffect(Player* player) { player->activateRemote(); }