#pragma once
#include "PowerUp.h"


class RemoteController : public PowerUp {
public:
    // FUNCIONES DE OBJETO
    RemoteController();
    void init(glm::vec2 pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram);

    // FUNCIONES AUXILIARES
    void applyEffect(Player* player) override;
};