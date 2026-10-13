#include "Projectile.h"

Projectile::~Projectile() { if (sprite) delete sprite; }

void Projectile::update(int deltaTime, TileMap* map, glm::vec2 player_pos) {

}

void Projectile::render() {

}