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
#include "Boss.h"
#include "Menu.h"
#include "Cursor.h"
#include "Info.h"


enum class GameState { MENU, GAME };
enum class MenuState { MAIN, LVL1, LVL2, LVL3, LVL4, LVLBOSS, GAMEOVER };

class Scene
{
public:
	// FUNCIONES DE OBJETO
	Scene();
	~Scene();
	void init();
	
	// FUNCIONES DE JUEGO
	void init_level(int i);
	void update(int deltaTime);
	void render();
	
	// FUNCIONES AUXILIARES
	void setGodMode();
	void killAllEnemys();

	void enemy_killed();

private:
	void initShaders();

	void spawnPower(EntitySpawn t);
	

	glm::vec2 getCameraPosition() const;
	void updateCamera();

	MenuState Scene::lvl2menu(int i);
	
	

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
	Boss* boss;

	// Otros
	bool dropBombKeyPressed = false;
	glm::vec2 cameraPos;
	glm::vec2 playerSpawnPoint;

	// Elements de menú
	Menu* menu;
	Cursor* cursor;
	bool menuCanChange;

	// Elements del HUD
	Info* info;

	// Atributs de control
	int level;
	int level_time;
	int enemies;
	bool activeBoss;
	MenuState menu_state;
	GameState state;

};
#endif // _SCENE_INCLUDE

