#ifndef _MINION_INCLUDE
#define _MINION_INCLUDE


#include "Enemy.h"

enum class MinionState { ACTIVE, INACTIVE };

class Minion : public Enemy {
public:
    // FUNCIONES DE OBJETO
    void init(const glm::vec2& pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram) override;
    void render() override;
    void activate(glm::vec2 pos, bool direction);
    MinionState getState();

protected:
    // FUNCIONES DE JUEGO
    void updateLogic(int deltaTime, TileMap* map, glm::vec2 player_pos) override;

private:
    // FUNCIONES AUXILIARES
    void applyGravity(TileMap* map);

private:
    // Atributos
    float pauseTimer = 0.0f;
    bool movingRight;
    MinionState inBossState;
};

#endif