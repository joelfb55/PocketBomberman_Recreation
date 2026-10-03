#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Sprite.h"
#include "TileMap.h"

enum ExplosionsAnims {  // Animaciones posibles
    CENTRO,
    MEDIO_V, MEDIO_H,
    FINAL_R, FINAL_L, FINAL_U, FINAL_D
};

class Explosion {
public:
    // FUNCIONES DE OBJETO
    Explosion();
    ~Explosion();
    void init(const glm::ivec2& centerTile, int radius, ShaderProgram& shaderProgram, TileMap* map);
    
    // FUNCIONES DE JUEGO
    void update(int deltaTime);
    void render() const;

    // FUNCIONES AUXILIARES
    bool isFinished() const;
    bool hitsTile(const glm::ivec2& tilePos) const;

private:
    void calculateExpansion(const glm::ivec2& center, int radius, ShaderProgram& shaderProgram, TileMap* map);
    Sprite* createFireSprite(ExplosionsAnims anim, const glm::ivec2& tilePos, ShaderProgram& shaderProgram);

private:
    // Render
    Texture spritesheet;
    std::vector<Sprite*> fireSprites;

    // Estado
    bool bFinished;

    // Atributos
    float timer;
    int tileSize;
    std::vector<glm::ivec2> affectedTiles;

};