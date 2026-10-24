#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


enum PlayerAnims {	// Animaciones posibles
	STAND, MOVE, JUMP, FALL, DEATH_BY_BOMB, DEATH_BY_ENEMY, WIN
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
	void activateHeart();
	void activateRemote();

	void die(bool by_bomb, int deltaTime);
	bool isDead() const;
	bool isDeathAnimationFinished() const;
	int getVidas() const;
	int getMaxBombs();
	bool isHeartActive();
	bool isRemoteActive();
	void reset();
	void gameOverReset();

	glm::vec2 getPosition();
	int getExplosionRadius();
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	void setGodMode();

	void win();
	
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
	bool remoteExplosion = false;

	// Atributos
	int jumpAngle, startY;
	bool canJump;

	int activeBombs = 0;
	int maxBombs = 1;
	int explosionRadius = 1;

	bool heart = false;
	bool immunity = false;
	float heart_immunity;
	bool visible;
	int visible_time = 0, invisible_time = 0;

	int vidas = 3;
	float deathTimer = 0.f;
	bool win_state;

	float vertical_velocity;

};

#endif // _PLAYER_INCLUDE


