#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Menu.h"
#include "Heart.h"
#include "RemoteController.h"
#include "Boss.h"


#define SCREEN_X 16
#define SCREEN_Y 16

#define VIRTUAL_WIDTH  180.0f
#define VIRTUAL_HEIGHT 126.0f

#define MENU_IN_TIME 1000.f
#define MENU_OUT_TIME 1000.f

#define CELEBRATION_TIME 5000.f;


// FUNCIONES DE OBJETO
Scene::Scene(){
	map = NULL;
	player = NULL;

	cameraPos = glm::vec2(0.f, 0.f);
}
Scene::~Scene(){
	texProgram.free();
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;

	for (Bomba* bomb : activeBombs) {
		delete bomb;
	}
	activeBombs.clear();

	for (Explosion* expl : activeExplosions) {
		delete expl;
	}
	activeExplosions.clear();

	for (PowerUp* pUp : activePowerUps) {
		delete pUp;
	}
	activePowerUps.clear();

	for (Enemy* e : activeEnemys) {
		delete e;
	}
	activeEnemys.clear();

	delete boss;
}

void Scene::init(){
	initShaders();
	menu = new Menu();
	menu->init(texProgram);
	menu->setImage(0);
	cursor = new Cursor();
	cursor->init();
	info = new Info();
	info->init(texProgram);
	state = GameState::MENU;
	menu_state = MenuState::MAIN;
	menuCanChange = true;
	explode_first_bomb = false;
	remote_cooldown = false;
	level = 1;
	activeBoss = false;
	door_closed = true;
}

void Scene::initShaders() {
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if (!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}


MenuState Scene::lvl2menu(int i) {
	switch (i) {
	case 1: return MenuState::LVL1;
	case 2: return MenuState::LVL2;
	case 3: return MenuState::LVL3;
	case 4: return MenuState::LVL4;
	case 5: return MenuState::LVLBOSS;
	}
}

void Scene::init_level(int i) {

	// Reset timer;
	level_time = 9*60000; // 3 mins en ms

	// Reset Bombas
	for (Bomba* bomb : activeBombs) delete bomb;
	activeBombs.clear();
	for (Explosion* expl : activeExplosions) delete expl;
	activeExplosions.clear();

	// Reset enemigos
	killAllEnemys();
	for (Enemy* e : activeEnemys) delete e;
	activeEnemys.clear();

	// Reset PowerUps
	for (PowerUp* p : activePowerUps) delete p;
	activePowerUps.clear();

	level = i;
	menu_state = lvl2menu(i);
	menu->setImage(i);
	state = GameState::MENU;
	enemies = 0;
	activeBoss = false;

	// Reset puerta
	door_closed = true;

	// Mapa
	map = TileMap::createTileMap("levels/level" + to_string(i) + ".txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	map->setActiveBombs(&activeBombs);

	// Entidades
	const auto& spawns = map->getEntitySpawns();
	int tSize = map->getTileSize();

	for (const EntitySpawn& spawn : spawns) {
		glm::vec2 pixelPos = glm::vec2(spawn.tileCoords.x * tSize, spawn.tileCoords.y * tSize);

		switch (spawn.type) {
		case EntityType::PLAYER_SPAWN: {
			if (player == nullptr) player = new Player();
			player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
			player->setTileMap(map);
			playerSpawnPoint = pixelPos;
			player->setPosition(playerSpawnPoint);
			break;
		}

		case EntityType::WALKER_ENEMY: {
			WalkerEnemy* enemy1 = new WalkerEnemy();
			enemy1->init(pixelPos, glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
			activeEnemys.push_back(enemy1);
			++enemies;
			break;
		}

		case EntityType::FLOATING_ENEMY: {
			FloatingEnemy* enemy1 = new FloatingEnemy();
			enemy1->init(pixelPos, glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
			activeEnemys.push_back(enemy1);
			++enemies;
			break;
		}

		case EntityType::GHOST_ENEMY: {
			GhostEnemy* enemy1 = new GhostEnemy();
			enemy1->init(pixelPos, glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
			activeEnemys.push_back(enemy1);
			++enemies;
			break;
		}
		case EntityType::BOSS: {
			boss = new Boss();
			boss->init(pixelPos, glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
			activeBoss = true;
			break;
		}
		}
	}

	// Inicializar la camara centrada en el jugador
	glm::vec2 playerPos = player->getPosition();
	cameraPos.x = playerPos.x - (VIRTUAL_WIDTH / 2.0f);
	cameraPos.y = playerPos.y - (VIRTUAL_HEIGHT / 2.0f);

	currentTime = 0.0f;
	//cout << enemies << endl;

	player->reset();
	updateCamera();

	// Para la transición
}

void Scene::main_menu() {
	state = GameState::MENU;
	menu->setImage(0);
	menu_state = MenuState::MAIN;
}

void Scene::spawnPower(EntitySpawn spawn) {
	int tSize = map->getTileSize();
	glm::vec2 pixelPos = glm::vec2(spawn.tileCoords.x * tSize, spawn.tileCoords.y * tSize);
	switch (spawn.type) {
	case EntityType::BOMB_UP: {
		BombUp* power = new BombUp();
		power->init(pixelPos, glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
		activePowerUps.push_back(power);
		break;
	}
	case EntityType::FIRE_UP: {
		FireUp* power = new FireUp();
		power->init(pixelPos, glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
		activePowerUps.push_back(power);
		break;
	}
	case EntityType::HEART: {
		Heart* power = new Heart();
		power->init(pixelPos, glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
		activePowerUps.push_back(power);
		break;
	}
	case EntityType::REMOTE_CONTROLLER: {
		RemoteController* power = new RemoteController();
		power->init(pixelPos, glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
		activePowerUps.push_back(power);
		break;
	}
	}
}


// FUNCIONES DE JUEGO
void Scene::update(int deltaTime) {
	currentTime += deltaTime;

	if (menu_out_time > 0) {
		menu_out_time -= deltaTime;
		menuCanChange = false;
	}

	if (state == GameState::MENU) {
		if (menu_state == MenuState::MAIN) cursor->update(deltaTime);
		if (Game::instance().getKey(GLFW_KEY_ENTER) && menuCanChange) {
			state = GameState::MENU_IN;
			menuCanChange = false;
			menu_in_time = MENU_IN_TIME;
		}
		if (!menuCanChange && !Game::instance().getKey(GLFW_KEY_ENTER)) menuCanChange = true;
	}

	else if (state == GameState::MENU_IN) {
		if (menu_in_time < 0) {
			state = GameState::MENU;
			menu_out_time = MENU_OUT_TIME;
			if (menu_state == MenuState::MAIN){
				Pointing aux = cursor->getPointing();
				if (aux == Pointing::PLAY) {
					menu->setImage(1);
					init_level(1);
				}
				else if (aux == Pointing::CREDITS) {
					menu->setImage(8);
					menu_state = MenuState::CREDITS;
				}
				else if (aux == Pointing::INST) {
					menu->setImage(7);
					menu_state = MenuState::INSTR;
				}
			}
			else if (menu_state == MenuState::CREDITS || menu_state == MenuState::INSTR || menu_state == MenuState::WIN) {
				main_menu();
			}
			else if (menu_state == MenuState::GAMEOVER) {
				menu->setImage(0);
				menu_state = MenuState::MAIN;
				level = 1;
			}
			else state = GameState::GAME;
		}
     	menu_in_time -= deltaTime;
	}

	else if (state == GameState::MENU_OUT_GAME) {
		if (menu_in_time < 0) {
			if (menu_state == MenuState::WIN) {
				state = GameState::MENU;
				menu->setImage(10);
			}
			else if (player->getVidas() <= 0) {
				state = GameState::MENU;
				menu_state = MenuState::GAMEOVER;
				menu->setImage(6);
				player->gameOverReset();
			}
			else {
				init_level(level);
			}
			menu_out_time = MENU_OUT_TIME;
		}
		menu_in_time -= deltaTime;
	}

	else if (state == GameState::GAME) {

		level_time -= deltaTime;
		if (level_time < 0) {
			player->die(false, deltaTime);
			level_time = 1 * 60000;
		}

		// Si el player quiere poner bomba
		if (player->wantsToGenerateABomb()) {
			if (!dropBombKeyPressed) {
				dropBombKeyPressed = true; // Bloquea la creación hasta soltar la tecla

				if (player->canDropBomb()) {
					glm::ivec2 bombPos = player->getBombTilePosition(map->getTileSize());

					// Comprobar no haya otra en el sitio ya
					bool canBomb = true;
					for (Bomba* bomb : activeBombs) if (bomb->getTilePos() == bombPos) canBomb = false;

					if (canBomb) {
						Bomba* newBomba = new Bomba();
						newBomba->init(bombPos, texProgram);

						activeBombs.push_back(newBomba);
						player->registerBomb();
					}

				}
			}
		}
		else {
			dropBombKeyPressed = false;
		}

		// Actualizar enemigos
		for (auto it = activeEnemys.begin(); it != activeEnemys.end(); ) {
			(*it)->update(deltaTime, map, player->getPosition());

			// Daño al jugador
			if ((*it)->checkCollision(player->getPosition(), glm::ivec2(16, 16))) {
				player->die(false, deltaTime);
			}

			if (!(*it)->isAlive()) {
				delete* it;
				it = activeEnemys.erase(it);
			}
			else {
				++it;
			}
		}

		// Actualizar bombas activas
		if (!remote_cooldown && activeBombs.size() > 0 && Game::instance().getKey(GLFW_KEY_C)) {
			explode_first_bomb = true;
			remote_cooldown = true;
		}
		for (auto it = activeBombs.begin(); it != activeBombs.end();) {
			(*it)->updatePlayerOverlap(player->getPosition(), glm::ivec2(16, 16), map->getTileSize());

			(*it)->update(deltaTime);
			if ((*it)->isPrematureExploded()) {
				player->onBombExploded();

				Explosion* expl = new Explosion();
				expl->init((*it)->getTilePos(), player->getExplosionRadius(), texProgram, map);
				activeExplosions.push_back(expl);

				delete* it;
				it = activeBombs.erase(it);
			}
			else {
				if (player->isRemoteActive()) {
					if (explode_first_bomb) {
						player->onBombExploded();
						Explosion* expl = new Explosion();
						expl->init((*it)->getTilePos(), player->getExplosionRadius(), texProgram, map);
						activeExplosions.push_back(expl);

						delete* it;
						it = activeBombs.erase(it);
						explode_first_bomb = false;
					}
					else {
						++it;
					}
				}
				else {
					if ((*it)->isExploded()) {
						player->onBombExploded();

						Explosion* expl = new Explosion();
						expl->init((*it)->getTilePos(), player->getExplosionRadius(), texProgram, map);
						activeExplosions.push_back(expl);

						delete* it;
						it = activeBombs.erase(it);
					}
					else {
						++it;
					}
				}
			}
		}
		if (!Game::instance().getKey(GLFW_KEY_C)) {
			remote_cooldown = false;
		}

		// Boss
		if (activeBoss) {
			boss->update(deltaTime, map, player->getPosition());
			Minion* e = boss->getLeftMinion();
			if (e->getState() == MinionState::ACTIVE && e->checkCollision(player->getPosition(), glm::ivec2(16, 16))) {
				player->die(false, deltaTime);
			}
			e = boss->getRightMinion();
			if (e->getState() == MinionState::ACTIVE && e->checkCollision(player->getPosition(), glm::ivec2(16, 16))) {
				player->die(false, deltaTime);
			}
			if (boss->checkCollision(player->getPosition(), glm::ivec2(16, 16))) { // Tambien checkea los proyectiles.
				player->die(false, deltaTime);
			}
			if (boss->isDead()) {
				state = GameState::WIN;
				menu_state = MenuState::WIN;
				menu->setImage(10);
				celebration_time = CELEBRATION_TIME;
				player->win();
			}
		}

		// Actualizar explosiones
		for (auto it = activeExplosions.begin(); it != activeExplosions.end();) {
			(*it)->update(deltaTime);

			// Spawneamos todos los powerups que se hayan descubierto con la explosión
			std::queue<EntitySpawn> powers = map->getPowerUpSpawns();
			while (!powers.empty()) {
				std::cout << powers.size() << endl;
				spawnPower(powers.front());
				powers.pop();
			}
			map->resetPowers();

			// Deteccion daño a otras bombas
			for (auto b = activeBombs.begin(); b != activeBombs.end(); ++b) {
				if ((*it)->hitsTile((*b)->getTilePos())){
					(*b)->premature_explode();
				}
			}

			// Deteccion daño player
			if ((*it)->hitsTile(player->getBombTilePosition(map->getTileSize()))) {
				player->die(true, deltaTime);
			}

			// Daño al enemigo por explosiones
			for (Enemy* e : activeEnemys) {
				if (!e->isReallyAlive()) continue;

				// Comprobamos si alguno de los tiles que ocupa el enemigo es alcanzado por la explosión
				const auto occupiedTiles = e->getOccupiedTilePositions(map->getTileSize()); // para los imprecisos
				for (const glm::ivec2& tilePixelPos : occupiedTiles) {
					if ((*it)->hitsTile(tilePixelPos)) {
						e->kill();
						--enemies;
						break;
					}
				}
			}

			// Detección daño al Boss y esbirros
			if (activeBoss) {
				if (boss->touchesExplosion((*it)->getAffectedTiles(), map->getTileSize())) boss->getsDamage();
				if (boss->getLeftMinion()->getState() == MinionState::ACTIVE) {
					Enemy* e = boss->getLeftMinion();
					const auto occupiedTiles = e->getOccupiedTilePositions(map->getTileSize()); // para los imprecisos
					for (const glm::ivec2& tilePixelPos : occupiedTiles) {
						if ((*it)->hitsTile(tilePixelPos)) {
							e->kill();
							break;
						}
					}
				}
				if (boss->getRightMinion()->getState() == MinionState::ACTIVE) {
					Enemy* e = boss->getRightMinion();
					const auto occupiedTiles = e->getOccupiedTilePositions(map->getTileSize()); // para los imprecisos
					for (const glm::ivec2& tilePixelPos : occupiedTiles) {
						if ((*it)->hitsTile(tilePixelPos)) {
							e->kill();
							break;
						}
					}
				}

			}

			if ((*it)->isFinished()) {
				delete* it;
				it = activeExplosions.erase(it);
			}
			else ++it;
		}

		// Poderes
		for (auto it = activePowerUps.begin(); it != activePowerUps.end(); ) {
			(*it)->update(deltaTime);

			if ((*it)->checkCollision(player->getPosition())) {
				(*it)->applyEffect(player);

				delete* it;
				it = activePowerUps.erase(it);
			}
			else {
				++it;
			}
		}

		// Muerte -> faltara cambiarlo para enseñar el game over bien i demas
		if (player->isDeathAnimationFinished()) { // Se traspassa la logica a el estado MENU_OUT_GAME
			state = GameState::MENU_OUT_GAME;
			menu_in_time = MENU_IN_TIME;
		}

		// Puerta
		if (enemies == 0 && door_closed) {
			map->open_door();
			door_closed = false;
		}
		if (!door_closed && map->doorCollision(player->getPosition())) { // Igual que arriba.
			++level;
			state = GameState::MENU_OUT_GAME;
			menu_in_time = MENU_IN_TIME;
		}

		player->update(deltaTime);

		// Actualitzem HUD
		info->update(deltaTime, player, level_time, enemies);

		// Para debug
		if (Game::instance().getKey(GLFW_KEY_J)) {
			cout << "x: " << player->getPosition().x << ", y: " << player->getPosition().y << endl;
		}
		updateCamera();
	}

	else if (state == GameState::WIN) {
		player->update(deltaTime);
		if (celebration_time > 0) celebration_time -= deltaTime;
		else {
			menu_in_time = MENU_IN_TIME;
			state = GameState::MENU_OUT_GAME;
		}

	}
}

void Scene::render(){

	if (state == GameState::MENU) {
		texProgram.use();
		texProgram.setUniformMatrix4f("projection", glm::mat4(1.0f));
		float val = 1.0f;
		if (menu_out_time > 0) val = 1 - menu_out_time / MENU_OUT_TIME;
		texProgram.setUniform4f("color", val, val, val, 1.0f);
		texProgram.setUniformMatrix4f("modelview", glm::mat4(1.0f));
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		menu->render();
		if (menu_state == MenuState::MAIN) {
			cursor->render();
		}
	}
	else if (state == GameState::MENU_IN) {
		texProgram.use();
		texProgram.setUniformMatrix4f("projection", glm::mat4(1.0f));
		float val = menu_in_time / MENU_IN_TIME;
		texProgram.setUniform4f("color", val, val, val, 1.0f);
		texProgram.setUniformMatrix4f("modelview", glm::mat4(1.0f));
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		menu->render();
	}
	else if (state == GameState::GAME || state == GameState::MENU_OUT_GAME || state == GameState::WIN) {
		// Camara respecto size virtual
		projection = glm::ortho(cameraPos.x, cameraPos.x + VIRTUAL_WIDTH,
			cameraPos.y + VIRTUAL_HEIGHT, cameraPos.y);
		glm::mat4 modelview;

		texProgram.use();
		texProgram.setUniformMatrix4f("projection", projection);
		float val = 1.0f;
		if (menu_out_time > 0) val = 1 - menu_out_time / MENU_OUT_TIME;
		if (state == GameState::MENU_OUT_GAME) val = menu_in_time / MENU_IN_TIME;
		texProgram.setUniform4f("color", val, val, val, 1.0f);
		modelview = glm::mat4(1.0f);
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		map->render();

		// Bombas
		for (Bomba* bomb : activeBombs) { bomb->render(); }

		// Explosiones
		for (Explosion* expl : activeExplosions) { expl->render(); }

		// Power Ups
		for (PowerUp* pUp : activePowerUps) { pUp->render(); }

		//Enemigos
		for (Enemy* enemy : activeEnemys) { enemy->render(); }

		// Boss
		if (activeBoss) {
			boss->render();
		}

		if(state == GameState::GAME || state == GameState::WIN) player->render();

		// HUD
		info->render();
	}
}

// FUNCIONES AUXILIARES
// Camara -> REVISAR
void Scene::updateCamera(){
	// Centro  personaje
	float tileSize = static_cast<float>(map->getTileSize());
	glm::vec2 playerCenter = player->getPosition() + glm::vec2(tileSize * 0.5f, tileSize * 0.5f);

	// Margenes
	float deadzoneLeft = cameraPos.x + (VIRTUAL_WIDTH * 0.25f); 
	float deadzoneRight = cameraPos.x + (VIRTUAL_WIDTH * 0.5f);
	float deadzoneTop = cameraPos.y + (VIRTUAL_HEIGHT * 0.3f);
	float deadzoneBottom = cameraPos.y + (VIRTUAL_HEIGHT * 0.35f);

	// Movimiento camara
	if (playerCenter.x < deadzoneLeft)
		cameraPos.x -= (deadzoneLeft - playerCenter.x);
	else if (playerCenter.x > deadzoneRight)
		cameraPos.x += (playerCenter.x - deadzoneRight);

	if (playerCenter.y < deadzoneTop)
		cameraPos.y -= (deadzoneTop - playerCenter.y);
	else if (playerCenter.y > deadzoneBottom)
		cameraPos.y += (playerCenter.y - deadzoneBottom);

	// Clamping
	float mapWidthPx = map->getMapDimensions().x * tileSize + SCREEN_X;
	float mapHeightPx = map->getMapDimensions().y * tileSize + SCREEN_Y;

	float maxCamX = std::max(0.0f, mapWidthPx - VIRTUAL_WIDTH);
	float maxCamY = std::max(0.0f, mapHeightPx - VIRTUAL_HEIGHT);

	cameraPos.x = glm::clamp(cameraPos.x, float(SCREEN_X), maxCamX);
	cameraPos.y = glm::clamp(cameraPos.y, float(SCREEN_Y), maxCamY);
}

// Trucos
void Scene::setGodMode() { player->setGodMode(); }
void Scene::killAllEnemys() {
	for (Enemy* e : activeEnemys) {
		e->kill();
		--enemies;
	}
	if (activeBoss) boss->kill();
	// Faltara abrir puerta o asi
}

void Scene::enemy_killed() {
	--enemies;
}
