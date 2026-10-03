#include "PowerUp.h"


class BombUp : public PowerUp {
public:
    // FUNCIONES DE OBJETO
    BombUp();
    void init(glm::vec2 pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram);
    
    // FUNCIONES AUXILIARES
    void applyEffect(Player* player) override;
};