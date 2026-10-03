#include "GhostEnemy.h"
#include <time.h>
#include <iostream>

#define ENEMY_SIZE 16.f

#define ENEMY_SIZE 16.f
#define ENEMY_MOVING_SPEED 0.5f
#define VISION 4*16.f

// FUNCIONES DE OBJETO
void GhostEnemy::init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) {
    srand((unsigned)time(NULL));
    random_direction = glm::vec2(my_rand(), my_rand());
    position = pos;
    tileDispl = tileMapDispl;
    state = EnemyState::ALIVE;

    spritesheet.loadFromFile("images/ghostEnemy.png", TEXTURE_PIXEL_FORMAT_RGBA);
    spritesheet.setWrapS(GL_CLAMP_TO_EDGE);
    spritesheet.setWrapT(GL_CLAMP_TO_EDGE);
    spritesheet.setMinFilter(GL_NEAREST);
    spritesheet.setMagFilter(GL_NEAREST);
    sprite = Sprite::createSprite(glm::ivec2(ENEMY_SIZE, ENEMY_SIZE), glm::vec2(0.25, 0.25f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(2);

    // Caminar
    sprite->setAnimationSpeed(0, 6);
    sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    sprite->addKeyframe(0, glm::vec2(0.25f, 0.f));
    sprite->addKeyframe(0, glm::vec2(0.5, 0.f));

    // Muerte
    sprite->setAnimationSpeed(1, 8);
    sprite->addKeyframe(1, glm::vec2(0.75f, 0.f));
    sprite->addKeyframe(1, glm::vec2(0.f, 0.25f));
    sprite->addKeyframe(1, glm::vec2(0.25, 0.25f));

    sprite->changeAnimation(0);
    sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
}

float GhostEnemy::my_rand() {
    return 2 * float(rand()) / RAND_MAX - 1;
}

// FUNCIONES DE JUEGO
void GhostEnemy::updateLogic(int deltaTime, TileMap* map, glm::vec2 player_pos) {
    float dist = glm::distance(position, player_pos);
    if (dist < VISION) { // Persecució (possibilitat de fer-la fantasma)

                        // Falta retocar, es queda encallat a vegades.

        float old_x = position.x;
        float old_y = position.y;

        glm::vec2 direction = (player_pos - position);
        position = position + ENEMY_MOVING_SPEED * direction/dist;
        random_direction = glm::vec2(my_rand(), my_rand()); // Para que después de seguirte no vaya en la misma direccion que antes (se puede quitar); 
        
        int pos_y = static_cast<int>(position.y); // Rarete
        if (direction.y < 0.f) {
            if (map->collisionMoveUp(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE), &pos_y)){
                position = glm::vec2(position.x, old_y);
            }
        }
        else if (direction.y > 0.f) {
            if (map->collisionMoveDown(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE), &pos_y)) {
                position = position = glm::vec2(position.x, old_y);
            }
        }
        if (direction.x < 0.f) {
            if (map->collisionMoveLeft(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE))) {
                position = glm::vec2(old_x, position.y);
            }
        }
        else if (direction.x > 0.f) {
            if (map->collisionMoveRight(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE))) {
                position = glm::vec2(old_x, position.y);
            }
        }
    }
    else {
        glm::vec2 old_pos = position;
        position += ENEMY_MOVING_SPEED * random_direction / glm::length(random_direction);
        int pos_y = static_cast<int>(position.y); // Rarete
        bool y_changed = false;
        if (random_direction.y < 0.f) {
            if (map->collisionMoveUp(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE), &pos_y)) {
                position = old_pos;
                random_direction = glm::vec2(my_rand(), -random_direction.y);
                y_changed = true;
            }
        }
        else if(random_direction.y > 0.f) {
            if (map->collisionMoveDown(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE), &pos_y)) {
                position = old_pos;
                random_direction = glm::vec2(my_rand(), -random_direction.y);
                y_changed = true;
            }
        }
        if (random_direction.x < 0.f) {
            if (map->collisionMoveLeft(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE))) {
                position = old_pos;
                float new_y = my_rand();
                if (y_changed) new_y = random_direction.y;
                random_direction = glm::vec2(-random_direction.x, new_y);
            }
        }
        else if(random_direction.x > 0.f){
            if (map->collisionMoveRight(glm::ivec2(position), glm::vec2(ENEMY_SIZE, ENEMY_SIZE))) {
                position = old_pos;
                float new_y = my_rand();
                if (y_changed) new_y = random_direction.y;
                random_direction = glm::vec2(-random_direction.x, new_y);
            }
        }
     }
    sprite->setPosition(glm::vec2(tileDispl.x + position.x, tileDispl.y + position.y));
    sprite->update(deltaTime);
}
