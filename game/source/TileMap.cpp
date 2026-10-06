#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"


using namespace std;


TileMap *TileMap::createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program){
	TileMap *map = new TileMap(levelFile, minCoords, program);
	
	return map;
}

TileMap::TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program){
	minTileCoords = minCoords;
	
	loadLevel(levelFile);
	prepareArrays(minCoords, program);

	currentProgramPtr = &program;
}

TileMap::~TileMap(){
	if(map != NULL)
		delete [] map;
	free();
}


void TileMap::render() const{
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * nTiles);
	glDisable(GL_TEXTURE_2D);
}

void TileMap::free(){
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

bool TileMap::loadLevel(const string &levelFile){
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;
	
	fin.open(levelFile.c_str());
	if(!fin.is_open())
		return false;
	getline(fin, line);
	if(line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);
	
	map = new int[mapSize.x * mapSize.y];
	powerups = new EntityType[mapSize.x * mapSize.y, EntityType::NOTHING];
	for(int j=0; j<mapSize.y; j++)
	{
		for(int i=0; i<mapSize.x; i++)
		{
			fin.get(tile);

			// Bloques
			if (tile == ' ') {
				map[j * mapSize.x + i] = 0;
			}
			else if (tile >= '0' && tile <= '9') {
				map[j * mapSize.x + i] = tile - '0';
			}
			else if(tile == 'a') map[j * mapSize.x + i] = 11;
			else if (tile == 'b') map[j * mapSize.x + i] = 12;

			// Puerta
			else if (tile == 'D') {
				map[j * mapSize.x + i] = 9;
				door_offset = j * mapSize.x + i;
				door_pixel[0] = i * tileSize;
				door_pixel[1] = j * tileSize;
				//cout << door_pixel[0] << " " << door_pixel[1] << endl;
			}

			// Entidades
			else if (tile == 'G' || tile == 'W' || tile == 'Y' || tile == 'P' || tile == 'O') {
				map[j * mapSize.x + i] = 0;
				EntitySpawn spawn;
				spawn.tileCoords = glm::ivec2(i, j);

				if (tile == 'W') spawn.type = EntityType::WALKER_ENEMY;
				else if (tile == 'Y') spawn.type = EntityType::FLOATING_ENEMY;
				else if (tile == 'G') spawn.type = EntityType::GHOST_ENEMY;
				else if (tile == 'P') spawn.type = EntityType::PLAYER_SPAWN;
				else if (tile == 'O') spawn.type = EntityType::BOSS;
				spawns.push_back(spawn);
			}

			// PowerUps
			else if (tile == 'B') {
				map[j * mapSize.x + i] = 8;
				powerups[j * mapSize.x + i] = EntityType::BOMB_UP;
			}
			else if (tile == 'F') {
				map[j * mapSize.x + i] = 8;
				powerups[j * mapSize.x + i] = EntityType::FIRE_UP;
			}
			else if (tile == 'H') {
				map[j * mapSize.x + i] = 8;
				powerups[j * mapSize.x + i] = EntityType::HEART;
			}
			else if (tile == 'R') {
				map[j * mapSize.x + i] = 8;
				powerups[j * mapSize.x + i] = EntityType::REMOTE_CONTROLLER;
			}


		}
		fin.get(tile);
		#ifndef _WIN32
		fin.get(tile);
		#endif
	}
	fin.close();
	
	return true;
}

void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program){
	int tile;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;
	
	nTiles = 0;
	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());
	for(int j=0; j<mapSize.y; j++)
	{
		for(int i=0; i<mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];
			
				// Non-empty tile
				nTiles++;
				posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
				texCoordTile[0] = glm::vec2(float((tile)%tilesheetSize.x) / tilesheetSize.x, float((tile)/tilesheetSize.x) / tilesheetSize.y);
				texCoordTile[1] = texCoordTile[0] + tileTexSize;
				//texCoordTile[0] += halfTexel;
				//texCoordTile[1] -= halfTexel;
				// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
			
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4*sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4*sizeof(float), (void *)(2*sizeof(float)));
}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below. -> IMPORTANTE

bool TileMap::collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size) const{
	int x, y0, y1;
	
	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for(int y=y0; y<=y1; y++){
		if(isTileSolid(x,y))
			return true;
	}
	
	return false;
}
bool TileMap::collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size) const{
	int x, y0, y1;
	
	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for(int y=y0; y<=y1; y++){
		if(isTileSolid(x, y))
			return true;
	}
	
	return false;
}
bool TileMap::collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, int *posY) const{
	int x0, x1, y;
	
	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;
	for(int x=x0; x<=x1; x++){
		if(isTileSolid(x, y)){
			if(*posY - tileSize * y + size.y <= 4){
				*posY = tileSize * y - size.y;
				return true;
			}
		}
	}
	
	return false;
}

bool TileMap::collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size, int* posY) const{
	int x0, x1, y;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = pos.y / tileSize;
	for (int x = x0; x <= x1; x++){
		if (isTileSolid(x, y)){
			*posY = (y + 1) * tileSize;
			return true;
		}
	}

	return false;
}


bool TileMap::doorCollision(glm::vec2& playerCoords) {
	return playerCoords.x == door_pixel.x && playerCoords.y == door_pixel.y;
}

//---

glm::vec2 TileMap::getMapDimensions() const{
	return mapSize;
}

void TileMap::setActiveBombs(const std::vector<Bomba*>* bombs) { activeBombs = bombs; }

bool TileMap::isTileSolid(int tileX, int tileY) const {
	// Límites del mapa
	if (tileX < 0 || tileX >= mapSize.x || tileY < 0 || tileY >= mapSize.y)
		return true;

	if (map[tileY * mapSize.x + tileX] == 10) return false;

	// Bloques del mapa
	if (map[tileY * mapSize.x + tileX] != 0)
		return true;

	// Bombas
	if (activeBombs != nullptr) {
		for (const Bomba* bomb : *activeBombs) {
			if (bomb != nullptr) {
				glm::ivec2 bPos = bomb->getTilePos();
				int bTileX = bPos.x / tileSize;
				int bTileY = bPos.y / tileSize;

				// Solo se comporta como obstáculo si el jugador ya no está dentro de ella
				if (bTileX == tileX && bTileY == tileY && bomb->isSolid())
					return true;
			}
		}
	}

	return false;
}

//---

const std::vector<EntitySpawn>& TileMap::getEntitySpawns() const { return spawns; }

const std::queue<EntitySpawn>& TileMap::getPowerUpSpawns() const { return powerUpSpawns; }

//---
bool TileMap::isWallTile(int tileX, int tileY) const {
	if (tileX < 0 || tileX >= mapSize.x || tileY < 0 || tileY >= mapSize.y)
		return true; // Fuera de límites cuenta como muro infranqueable
	return map[tileY * mapSize.x + tileX] != 8 && map[tileY * mapSize.x + tileX] != 0;
}

bool TileMap::isDestructibleTile(int tileX, int tileY) const {
	if (tileX < 0 || tileX >= mapSize.x || tileY < 0 || tileY >= mapSize.y)
		return false;
	return map[tileY * mapSize.x + tileX] == 8; // 2 = Bloque destructible
}

void TileMap::open_door() {
	map[door_offset] = 10;
	rebuildVBO();
}

bool TileMap::destroyTile(int tileX, int tileY) {
	if (isDestructibleTile(tileX, tileY)) {
		map[tileY * mapSize.x + tileX] = 0;
		if (powerups[tileY * mapSize.x + tileX] != EntityType::NOTHING) {
			EntitySpawn spawn;
			spawn.tileCoords = glm::ivec2(tileX, tileY);
			spawn.type = powerups[tileY * mapSize.x + tileX];
			powerUpSpawns.push(spawn);
		}
		rebuildVBO();
		return true;
	}
	return false;
}

void TileMap::rebuildVBO() {
	free();
	prepareArrays(minTileCoords, *currentProgramPtr);
}

void TileMap::resetPowers() {
	std::queue<EntitySpawn>().swap(powerUpSpawns);
}
