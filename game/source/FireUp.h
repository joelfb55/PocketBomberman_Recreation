#include "PowerUp.h"

class FireUp : public PowerUp {
public:
    // FUNCIONES DE OBJETO
    FireUp();
    void init(glm::vec2 pos, const glm::ivec2& tileMapDispl, ShaderProgram& shaderProgram);
    
    // FUNCIONES AUXILIARES
    void applyEffect(Player* player) override;
};