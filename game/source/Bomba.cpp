#include "Bomba.h"


#define EXPLOSION_TIME 3000.f	// Tiempo de que tarda en explotar la bomba

// FUNCIONES DE OBJETO
Bomba::Bomba(){
	sprite = nullptr;
	timer = EXPLOSION_TIME;
	bExploded = false;
}
Bomba::~Bomba(){
	if (sprite != nullptr)
		delete sprite;
}
void Bomba::init(const glm::ivec2& tilePos, ShaderProgram& shaderProgram){
	tilePosition = tilePos;
	
	if (!spritesheet.loadFromFile("images/bomba.png", TEXTURE_PIXEL_FORMAT_RGBA)) {
		printf("Error: fallida al intentar cargar la imagen");
	}
	spritesheet.setWrapS(GL_CLAMP_TO_EDGE);
	spritesheet.setWrapT(GL_CLAMP_TO_EDGE);
	spritesheet.setMinFilter(GL_NEAREST);
	spritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 1.f), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(1);
	
	// Tic - Tac
	sprite->setAnimationSpeed(0, 5);
	sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
	sprite->addKeyframe(0, glm::vec2(0.25f, 0.f));
	sprite->addKeyframe(0, glm::vec2(0.5f, 0.f));

	sprite->changeAnimation(0);
	sprite->setPosition(glm::vec2(float(tilePosition.x + 16.f), float(tilePosition.y + 16.f)));
}

// FUNCIONES DE JUEGO
void Bomba::update(int deltaTime){
	//if (bExploded) return;

	sprite->update(deltaTime);

	if (!bExploded) {
		timer -= float(deltaTime);
		if (timer <= 0.f) {
			bExploded = true;
		}
	}
}
void Bomba::render() const{
	if (sprite != nullptr){
		sprite->render();
	}
}


// FUNCIONES AUXILIARES
// Estados
bool Bomba::isExploded() const { return bExploded; }
bool Bomba::isPrematureExploded() const { return prematureExpl; }
bool Bomba::isSolid() const { return !playerInside; }

// Getters
glm::ivec2 Bomba::getTilePos() const { return tilePosition; }

// Otras
void Bomba::updatePlayerOverlap(const glm::vec2& playerPos, const glm::ivec2& playerSize, int tileSize) {
	if (!playerInside) return;

	bool overlapX = (playerPos.x < tilePosition.x + tileSize) && (playerPos.x + playerSize.x > tilePosition.x);
	bool overlapY = (playerPos.y < tilePosition.y + tileSize) && (playerPos.y + playerSize.y > tilePosition.y);

	// Jugador sale
	if (!overlapX || !overlapY) {
		playerInside = false;
	}
}

void Bomba::premature_explode() {
	prematureExpl = true;
}

