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

#define VIRTUAL_WIDTH 160.0f
#define VIRTUAL_HEIGHT 112.0f


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
	state = GameState::MENU;
	menu_state = 0;
	level = 1;
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

void Scene::init_level(int i) {

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
	menu_state = i;
	state = GameState::MENU;
	enemies = 0;
	activeBoss = false;

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
	cout << enemies << endl;

	player->reset();
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

	if (state == GameState::MENU) {
		if (Game::instance().getKey(GLFW_KEY_ENTER)) {
			if (menu_state == 0) {
				init_level(1);
			}
			else if (menu_state == 6) {
				menu_state = 0;
				level = 1;
			}
			else state = GameState::GAME;
		}
	}

	else if (state == GameState::GAME) {

		// Spawneamos todos los powerups que se hayan descubierto.
		std::queue<EntitySpawn> powers = map->getPowerUpSpawns();
		while (!powers.empty()) {
			spawnPower(powers.front());
			powers.pop();			
		}
		map->resetPowers();

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
		for (auto it = activeBombs.begin(); it != activeBombs.end();) {
			(*it)->updatePlayerOverlap(player->getPosition(), glm::ivec2(16, 16), map->getTileSize());

			(*it)->update(deltaTime);

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

		// Actualizar explosiones
		for (auto it = activeExplosions.begin(); it != activeExplosions.end();) {
			(*it)->update(deltaTime);

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
						cout << enemies << endl;
						break;
					}
				}
			}

			if ((*it)->isFinished()) {
				delete* it;
				it = activeExplosions.erase(it);
			}
			else ++it;
		}

		// Boss
		if (activeBoss) {
			boss->update(deltaTime, map, player->getPosition());
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
		if (player->isDeathAnimationFinished()) {
			if (player->getVidas() <= 0) {
				std::cout << "Game Over" << std::endl;
				state = GameState::MENU;
				menu_state = 6;
				player->gameOverReset();
			}
			else {
				init_level(level);
			}
		}

		// Puerta
		if (enemies == 0) {
			map->open_door();
			if (map->doorCollision(player->getPosition())) {
				++level;
				init_level(level);
			}
		}

		player->update(deltaTime);

		// Para debug
		if (Game::instance().getKey(GLFW_KEY_J)) {
			cout << "x: " << player->getPosition().x << ", y: " << player->getPosition().y << endl;
		}
		updateCamera();
	}
}

void Scene::render(){

	if (state == GameState::MENU) {
		Menu actual(menu_state, texProgram);
		texProgram.use();
		texProgram.setUniformMatrix4f("projection", glm::mat4(1.0f));
		texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		texProgram.setUniformMatrix4f("modelview", glm::mat4(1.0f));
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		actual.render();
	}
	else if (state == GameState::GAME) {
		// Camara respecto size virtual
		projection = glm::ortho(cameraPos.x, cameraPos.x + VIRTUAL_WIDTH,
			cameraPos.y + VIRTUAL_HEIGHT, cameraPos.y);
		glm::mat4 modelview;

		texProgram.use();
		texProgram.setUniformMatrix4f("projection", projection);
		texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
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

		player->render();
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
	float deadzoneTop = cameraPos.y + (VIRTUAL_HEIGHT * 0.35f);
	float deadzoneBottom = cameraPos.y + (VIRTUAL_HEIGHT * 0.6f);

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
	// Faltara abrir puerta o asi
}

void Scene::enemy_killed() {
	--enemies;
}
