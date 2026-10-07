#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"

// Esta clase es llamada por el main para generar el juego
// Basicamente se encarga de avisar a la escena de los inputs
// de las teclas y de llamar cada fraccion de segundo a UPDATE

void Game::init(){
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	scene.init();
}

bool Game::update(int deltaTime){
	scene.update(deltaTime);

	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
}

#include<iostream>
void Game::keyPressed(int key)
{
	if(key == GLFW_KEY_ESCAPE) // Escape code
		bPlay = false;
	// Añadido para los comandos especiales
	if (key == GLFW_KEY_G)
		scene.setGodMode();
	if (key == GLFW_KEY_K)
		scene.killAllEnemys();
	if (key == GLFW_KEY_1)
		scene.init_level(1);
	if (key == GLFW_KEY_2)
		scene.init_level(2);
	if (key == GLFW_KEY_3)
		scene.init_level(3);
	if (key == GLFW_KEY_4)
		scene.init_level(4);
	if (key == GLFW_KEY_5)
		scene.init_level(5);
	//std::cerr << "pressed " << key << std::endl;
	keys[key] = true;
}

void Game::keyReleased(int key)
{
	//std::cerr << "released " << key << std::endl;
	keys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const
{
	return keys[key];
}



