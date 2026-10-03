#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


enum PlayerAnims {	// Animaciones posibles
	STAND, MOVE, JUMP, FALL, DEATH_BY_BOMB, DEATH_BY_ENEMY
};

class Player
{
public:
	// FUNCIONES DE OBJETO
	Player();
	~Player();
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	
	// FUNCIONES DE JUEGO
	void update(int deltaTime);
	void render();
	
	// FUNCIONES AUXILIARES
	bool wantsToGenerateABomb() const;
	glm::ivec2 getBombTilePosition(int tileSize) const;
	bool canDropBomb() const;
	void registerBomb();
	void onBombExploded();

	void addMaxBomb();
	void addExplosionRadius();

	void die(bool by_bomb);
	bool isDead() const;
	bool isDeathAnimationFinished() const;
	int getVidas() const;
	void reset();

	glm::vec2 getPosition();
	int getExplosionRadius();
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	void setGodMode();
	
private:
	// Render
	glm::ivec2 tileMapDispl, posPlayer;
	Sprite* sprite;
	TileMap* map;
	Texture spritesheet;
	bool flipped = false;	// Indica si se hace espejo del sprite

	// Estados
	bool bJumping = false;
	bool bDead = false;
	bool godMode = false;

	// Atributos
	int jumpAngle, startY;

	int activeBombs = 0;
	int maxBombs = 1;
	int explosionRadius = 1;

	int vidas = 3;
	float deathTimer = 0.f;

};

#endif // _PLAYER_INCLUDE


