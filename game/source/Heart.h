#pragma once
#include "PowerUp.h"


class Heart : public PowerUp {
public:
    // FUNCIONES DE OBJETO
    Heart();
    void init(glm::vec2 pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram);

    // FUNCIONES AUXILIARES
    void applyEffect(Player* player) override;
};