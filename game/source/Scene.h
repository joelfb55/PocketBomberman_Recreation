#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"

#include "Bomba.h"
#include "Explosion.h"
#include "BombUp.h"
#include "FireUp.h"
#include "WalkerEnemy.h"
#include "FloatingEnemy.h"
#include "GhostEnemy.h"

class Scene
{
public:
	// FUNCIONES DE OBJETO
	Scene();
	~Scene();
	void init(int i);
	
	// FUNCIONES DE JUEGO
	void update(int deltaTime);
	void render();
	
	// FUNCIONES AUXILIARES
	void setGodMode();
	void killAllEnemys();
	void restartLevel(int i);

	void enemy_killed();
	void menu_control();

private:
	void initShaders();

	glm::vec2 getCameraPosition() const;
	void updateCamera();
	
	

private:
	// Render
	TileMap *map;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	// Entidades
	Player* player;

	std::vector<Bomba*> activeBombs;
	std::vector<Explosion*> activeExplosions;
	std::vector<Enemy*> activeEnemys;
	std::vector<PowerUp*> activePowerUps;

	// Otros
	bool dropBombKeyPressed = false;
	glm::vec2 cameraPos;
	glm::vec2 playerSpawnPoint;

	int level;
	int enemies;
	int menu = true;
	int menu_state = 0;

};
#endif // _SCENE_INCLUDE

