#include <iostream>
#include "Explosion.h"

#define LIFE_TIME 250.f

// FUNCIONES DE OBJETO
Explosion::Explosion(){
    timer = LIFE_TIME;
    bFinished = false;
}
Explosion::~Explosion() {
    for (Sprite* sprite : fireSprites) {
        delete sprite;
    }
    fireSprites.clear();
}
void Explosion::init(const glm::ivec2& centerTile, int radius, ShaderProgram& shaderProgram, TileMap* map) {
    tileSize = map->getTileSize();

    spritesheet.loadFromFile("images/explosion.png", TEXTURE_PIXEL_FORMAT_RGBA);
    spritesheet.setWrapS(GL_CLAMP_TO_EDGE);
    spritesheet.setWrapT(GL_CLAMP_TO_EDGE);
    spritesheet.setMinFilter(GL_NEAREST);
    spritesheet.setMagFilter(GL_NEAREST);


    // Calculamos rango
    calculateExpansion(centerTile, radius, shaderProgram, map);
}

// FUNCIONES DE JUEGO
void Explosion::update(int deltaTime) {
    if (bFinished) return;

    // Actualizar
    for (Sprite* sprite : fireSprites) {
        sprite->update(deltaTime);
    }

    timer -= float(deltaTime);
    if (timer <= 0.f) {
        bFinished = true;
    }
}
void Explosion::render() const {
    if (bFinished) return;

    for (Sprite* sprite : fireSprites) {
        sprite->render();
    }
}

// FUNCIONES AUXILIARES
// Estados
bool Explosion::isFinished() const { return bFinished; }
bool Explosion::hitsTile(const glm::ivec2& targetTile) const {
    for (const glm::ivec2& tile : affectedTiles) {
        if (tile == targetTile) return true;
    }
    return false;
}

// Otras
Sprite* Explosion::createFireSprite(ExplosionsAnims anim, const glm::ivec2& tilePos, ShaderProgram& shaderProgram) {
    Sprite* s = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(16.f / 112, 16.f / 48), &spritesheet, &shaderProgram);
    s->setNumberAnimations(7);

    s->setAnimationSpeed(CENTRO, 10);
    s->addKeyframe(CENTRO, glm::vec2(0.f, 0.f));
    s->addKeyframe(CENTRO, glm::vec2(0.f, 16.f / 48));
    s->addKeyframe(CENTRO, glm::vec2(0.f, 2 * 16.f / 48));

    s->setAnimationSpeed(MEDIO_H, 10);
    s->addKeyframe(MEDIO_H, glm::vec2(4 * 16.f / 112, 0.f));
    s->addKeyframe(MEDIO_H, glm::vec2(16.f / 112, 16.f / 48));
    s->addKeyframe(MEDIO_H, glm::vec2(16.f / 112, 2 * 16.f / 48));

    s->setAnimationSpeed(MEDIO_V, 10);
    s->addKeyframe(MEDIO_V, glm::vec2(6 * 16.f / 112.f, 0.f));
    s->addKeyframe(MEDIO_V, glm::vec2(2 * 16.f / 112, 16.f / 48));
    s->addKeyframe(MEDIO_V, glm::vec2(3 * 16.f / 112, 2 * 16.f / 48));

    s->setAnimationSpeed(FINAL_R, 10);
    s->addKeyframe(FINAL_R, glm::vec2(3 * 16.f / 112.f, 0.f));
    s->addKeyframe(FINAL_R, glm::vec2(3 * 16.f / 112.f, 16.f / 48));
    s->addKeyframe(FINAL_R, glm::vec2(3 * 16.f / 112.f, 2 * 16.f / 48));

    s->setAnimationSpeed(FINAL_L, 10);
    s->addKeyframe(FINAL_L, glm::vec2(5 * 16.f / 112.f, 0.f));
    s->addKeyframe(FINAL_L, glm::vec2(6 * 16.f / 112.f, 16.f / 48));
    s->addKeyframe(FINAL_L, glm::vec2(6 * 16.f / 112.f, 2 * 16.f / 48));

    s->setAnimationSpeed(FINAL_U, 10);
    s->addKeyframe(FINAL_U, glm::vec2(2 * 16.f / 112.f, 0.f));
    s->addKeyframe(FINAL_U, glm::vec2(5 * 16.f / 112.f, 16.f / 48));
    s->addKeyframe(FINAL_U, glm::vec2(5 * 16.f / 112.f, 2 * 16.f / 48));

    s->setAnimationSpeed(FINAL_D, 10);
    s->addKeyframe(FINAL_D, glm::vec2(16.f / 112.f, 0.f));
    s->addKeyframe(FINAL_D, glm::vec2(4 * 16.f / 112.f, 16.f / 48));
    s->addKeyframe(FINAL_D, glm::vec2(4 * 16.f / 112.f, 2 * 16.f / 48));

    s->changeAnimation(anim);
    s->setPosition(glm::vec2(float(tilePos.x + 16.f), float(tilePos.y + 16.f)));

    return s;
}
void Explosion::calculateExpansion(const glm::ivec2& center, int radius, ShaderProgram& shaderProgram, TileMap* map) {
    // Centro
    affectedTiles.push_back(center);
    fireSprites.push_back(createFireSprite(CENTRO, center, shaderProgram));

    // Dirs
    glm::ivec2 directions[4] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
    ExplosionsAnims endAnims[4] = { FINAL_R, FINAL_L, FINAL_D, FINAL_U };
    ExplosionsAnims midAnims[4] = { MEDIO_H, MEDIO_H, MEDIO_V, MEDIO_V };

    for (int d = 0; d < 4; ++d) {
        for (int i = 1; i <= radius; ++i) {
            glm::ivec2 currentTile = center + (directions[d] * (i * tileSize));
            int gridX = currentTile.x / tileSize;
            int gridY = currentTile.y / tileSize;

            // Choque pared
            if (map->isWallTile(gridX, gridY)) {
                break;
            }

            // Choque bloque destructible
            if (map->isDestructibleTile(gridX, gridY)) {
                cout << "yes" << endl;
                map->destroyTile(gridX, gridY);
            }

            affectedTiles.push_back(currentTile);

            // Comprobar la siguiente casilla para saber si dibuja final o medio
            glm::ivec2 nextTile = currentTile + (directions[d] * tileSize);
            int nextGridX = nextTile.x / tileSize;
            int nextGridY = nextTile.y / tileSize;

            bool nextIsWall = map->isWallTile(nextGridX, nextGridY);
            if (i == radius || nextIsWall) {
                fireSprites.push_back(createFireSprite(endAnims[d], currentTile, shaderProgram));
            }
            else {
                fireSprites.push_back(createFireSprite(midAnims[d], currentTile, shaderProgram));
            }
        }
    }
}
