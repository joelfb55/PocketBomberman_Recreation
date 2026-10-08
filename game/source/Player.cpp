#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"

#define PLAYER_SIZE 16
#define HITBOX_REDUCTION 0		// Indica cuantos pixeles quitamos al size para las colisiones de amplitud (x)
#define MOVEMENT_VELOCITY 1		// [pxls]

#define JUMP_ANGLE_STEP 4
#define FALL_STEP 4
#define JUMP_HEIGHT 3*16		// bloques * pixeles_bloque

#define HEART_IMMUNITY_TIME 1000.f

#define DEATH_ANIMATION_TIME 1000.f

// FUNCIONES DE OBJETO
Player::Player(){
	sprite = NULL;
	map = NULL;
}
Player::~Player(){
	if (sprite != NULL)
		delete sprite;
}
void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram){
	bJumping = false;
	bDead = false;
	heart = false;
	heart_immunity = 0.f;
	remoteExplosion = false;
	canJump = true;

	// Cargar textura
	spritesheet.loadFromFile("images/bomberman.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheet.setWrapS(GL_CLAMP_TO_EDGE);
	spritesheet.setWrapT(GL_CLAMP_TO_EDGE);
	spritesheet.setMinFilter(GL_NEAREST);
	spritesheet.setMagFilter(GL_NEAREST);

	// (pixeles personaje, tam celda[0,1], ... )
	sprite = Sprite::createSprite(glm::ivec2(PLAYER_SIZE, PLAYER_SIZE), glm::vec2(18.f/253, 18.f/632), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(6);
		
		// Parado
		sprite->setAnimationSpeed(STAND, 8);				// (ID, fps)
		sprite->addKeyframe(STAND, glm::vec2(0.f, 0.f));	// (ID, coords frame)
		
		// Corriendo
		sprite->setAnimationSpeed(MOVE, 10);
		sprite->addKeyframe(MOVE, glm::vec2(0.f, 0.f));
		sprite->addKeyframe(MOVE, glm::vec2(18.f / 253, 0.f));
		sprite->addKeyframe(MOVE, glm::vec2(35.f / 253, 0.f));

		// Saltando
		sprite->setAnimationSpeed(JUMP, 8);
		sprite->addKeyframe(JUMP, glm::vec2(51.f / 253, 0.f));
		sprite->addKeyframe(JUMP, glm::vec2(4 * 17.f / 253, 0.f));

		// Cayendo
		sprite->setAnimationSpeed(FALL, 8);
		sprite->addKeyframe(FALL, glm::vec2(5 * 17.f / 253, 0.f));

		// Muerte por bomba
		sprite->setAnimationSpeed(DEATH_BY_BOMB, 4);
		sprite->addKeyframe(DEATH_BY_BOMB, glm::vec2((3 * 18.f - 1) / 253, 18.f / 632));
		sprite->addKeyframe(DEATH_BY_BOMB, glm::vec2((4 * 18.f - 1) / 253, 18.f / 632));
		sprite->addKeyframe(DEATH_BY_BOMB, glm::vec2((5 * 18.f - 1) / 253, 18.f / 632));
		sprite->addKeyframe(DEATH_BY_BOMB, glm::vec2((6 * 18.f - 1) / 253, 18.f / 632));

		// Muerte por enemigo
		sprite->setAnimationSpeed(DEATH_BY_ENEMY, 8);
		sprite->addKeyframe(DEATH_BY_ENEMY, glm::vec2((7 * 18.f - 1) / 253, 18.f / 632));
		sprite->addKeyframe(DEATH_BY_ENEMY, glm::vec2(143.f / 253, 18.f / 632));

	// Activamos la primera animacion
	sprite->changeAnimation(0);

	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

// FUNCIONES DE JUEGO
void Player::update(int deltaTime){
	sprite->update(deltaTime);				// Actualiza la animacion activa a su siguiente KeyFrame (segun tiempo)

	// Muerto
	if (bDead) {
		deathTimer -= float(deltaTime);
		return;
	}

	if (immunity) {
		if (heart_immunity > HEART_IMMUNITY_TIME) {
			heart_immunity = 0;
			immunity = false;
		}
		else {
			heart_immunity += deltaTime;
		}
	}

	// Movimiento
	if(Game::instance().getKey(GLFW_KEY_LEFT)){
		flipped = false;
		sprite->setFlipped(flipped);
		
		if(sprite->animation() != MOVE)		// Si no se movia...
			sprite->changeAnimation(MOVE);

		posPlayer.x -= MOVEMENT_VELOCITY;
		if(map->collisionMoveLeft(posPlayer, glm::ivec2(PLAYER_SIZE - HITBOX_REDUCTION, PLAYER_SIZE))){
			posPlayer.x += MOVEMENT_VELOCITY;
			sprite->changeAnimation(STAND);
		}
	}
	else if(Game::instance().getKey(GLFW_KEY_RIGHT)){
		flipped = true;
		sprite->setFlipped(flipped);

		if(sprite->animation() != MOVE)
			sprite->changeAnimation(MOVE);

		posPlayer.x += MOVEMENT_VELOCITY;
		if(map->collisionMoveRight(posPlayer, glm::ivec2(PLAYER_SIZE - HITBOX_REDUCTION, PLAYER_SIZE))){
			posPlayer.x -= MOVEMENT_VELOCITY;
			sprite->changeAnimation(STAND);
		}
	}
	else { // Parar
		sprite->setFlipped(flipped);
		sprite->changeAnimation(STAND);
	}
	
	// Salto 
	if(bJumping){
		jumpAngle += JUMP_ANGLE_STEP;	// sumamos altura
		// Si hemos vuelto al suelo..
		if(jumpAngle == 180){			
			bJumping = false;
			posPlayer.y = startY;

			sprite->changeAnimation(STAND);
		}
		// Sino...
		else{
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			// si estamos cayendo
			if (jumpAngle > 90) {
				if (sprite->animation() != FALL)
					sprite->changeAnimation(FALL);

				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(PLAYER_SIZE, PLAYER_SIZE), &posPlayer.y);
				if(!bJumping) sprite->changeAnimation(STAND);
			
			}
			// si aun subimos
			else {
				if (sprite->animation() != JUMP)
					sprite->changeAnimation(JUMP);

				// Colision techo
				if (map->collisionMoveUp(posPlayer, glm::ivec2(PLAYER_SIZE - HITBOX_REDUCTION, PLAYER_SIZE), &posPlayer.y)){
					bJumping = false;
					sprite->changeAnimation(FALL);
				}
			}
		}
	}
	else{	// Aplicar gravedad si no saltas i comporbar si quiere saltar
		posPlayer.y += FALL_STEP;
		// Estamos en el suelo...
		if(map->collisionMoveDown(posPlayer, glm::ivec2(PLAYER_SIZE - HITBOX_REDUCTION, PLAYER_SIZE), &posPlayer.y)){
			if (sprite->animation() == FALL){
				sprite->changeAnimation(STAND);
			}

			// Si queremos saltar...
			if(Game::instance().getKey(GLFW_KEY_X) && canJump){
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
				sprite->changeAnimation(JUMP);
				canJump = false;
			}

			if (!canJump && !Game::instance().getKey(GLFW_KEY_X)) {
				canJump = true;
			}
		}
		// No estamos en el suelo
		else {
			if (sprite->animation() != FALL){
				sprite->changeAnimation(FALL);
			}
		}
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}
void Player::render(){
	sprite->render();
}

// FUNCIONES AUXILIARES
// Bombas (En orden de llamada)
bool Player::wantsToGenerateABomb() const { return Game::instance().getKey(GLFW_KEY_Z); }
bool Player::canDropBomb() const { return activeBombs < maxBombs; }
glm::ivec2 Player::getBombTilePosition(int tileSize) const {
	// Centro del sprite del jugador
	int centerX = posPlayer.x + tileSize / 4; // 16 / 2
	int centerY = posPlayer.y + tileSize / 4;

	//  Celda mas cercana
	int tileX = (centerX / tileSize) * tileSize;
	int tileY = (centerY / tileSize) * tileSize;

	return glm::ivec2(tileX, tileY);
}
void Player::registerBomb() { activeBombs++; }
void Player::onBombExploded() { activeBombs = glm::max(0, activeBombs - 1); }

// Poderes
void Player::addMaxBomb() { maxBombs++; }
void Player::addExplosionRadius() { explosionRadius++; }
void Player::activateHeart() { heart = true; }
void Player::activateRemote() { remoteExplosion = true; };


// Vidas
void Player::die(bool by_bomb, int deltaTime) {
	if (godMode || immunity) return;
	if (bDead) return;
	if (heart) {
		immunity = true;
		heart = false;
		return;
	}
	bDead = true;
	vidas--;
	deathTimer = DEATH_ANIMATION_TIME;
	if (by_bomb) sprite->changeAnimation(DEATH_BY_BOMB);
	else sprite->changeAnimation(DEATH_BY_ENEMY);
}
bool Player::isDeathAnimationFinished() const { return bDead && deathTimer <= 0.f; }
bool Player::isDead() const { return bDead; }
int Player::getVidas() const { return vidas; }

void Player::reset() {
	bDead = false;
	bJumping = false;
	activeBombs = 0;
	sprite->changeAnimation(STAND);
}


void Player::gameOverReset() {
	reset();
	maxBombs = 1;
	explosionRadius = 1;
	vidas = 3;
	heart = false;
	heart_immunity = 0;
	remoteExplosion = false;
}


// Getters // Setters
glm::vec2 Player::getPosition() { return posPlayer; }
int Player::getExplosionRadius() { return explosionRadius; }

void Player::setTileMap(TileMap* tileMap) { map = tileMap; }
void Player::setPosition(const glm::vec2& pos) {
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::setGodMode() { godMode = !godMode;}

int Player::getMaxBombs() { return maxBombs; }
bool Player::isHeartActive() { return heart; }
bool Player::isRemoteActive() { return remoteExplosion; }
