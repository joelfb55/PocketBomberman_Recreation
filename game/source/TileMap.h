#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include <queue>
#include "Texture.h"
#include "ShaderProgram.h"
#include "Bomba.h"

// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.
enum EntityType{NOTHING, PLAYER_SPAWN, WALKER_ENEMY, FLOATING_ENEMY, GHOST_ENEMY, BOMB_UP, FIRE_UP, HEART, REMOTE_CONTROLLER, BOSS};
struct EntitySpawn {
	EntityType type;
	glm::ivec2 tileCoords;
};

class TileMap
{

private:
	TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

	~TileMap();

	void render() const;
	void free();
	
	int getTileSize() const { return tileSize; }

	bool collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size) const;
	bool collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size) const;
	bool collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size, int* posY) const;
	bool collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, int *posY) const;

	bool collisionBorderMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionBorderMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionBorderMoveDown(const glm::ivec2& pos, const glm::ivec2& size, int* posY) const;
	bool collisionBorderMoveUp(const glm::ivec2& pos, const glm::ivec2& size, int* posY) const;

	//---
	glm::vec2 getMapDimensions() const;
	void setActiveBombs(const std::vector<Bomba*>* bombs);

	bool isWallTile(int tileX, int tileY) const;
	bool isDestructibleTile(int tileX, int tileY) const;
	bool destroyTile(int tileX, int tileY);
	void open_door();
	bool doorCollision(glm::vec2 &playerCoords);

	//---
	const std::vector<EntitySpawn>& getEntitySpawns() const;
	const std::queue<EntitySpawn>& getPowerUpSpawns() const;
	void resetPowers();
	
private:
	bool loadLevel(const string &levelFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);

	//---
	bool isTileSolid(int tileX, int tileY) const;
	bool isTileBorder(int tileX, int tileY) const;

	
	//---
	std::vector<EntitySpawn> spawns;
	std::queue<EntitySpawn> powerUpSpawns;

	//---
	void rebuildVBO();

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	int nTiles;
	glm::ivec2 position, mapSize, tilesheetSize;
	int tileSize, blockSize;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	int *map;
	EntityType *powerups;

	//----
	const std::vector<Bomba*>* activeBombs = nullptr;
	ShaderProgram *currentProgramPtr;

	//---
	glm::vec2 minTileCoords;

	bool door_presence;
	int door_offset;
	glm::ivec2 door_pixel;

	
};


#endif // _TILE_MAP_INCLUDE


