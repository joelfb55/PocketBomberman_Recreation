#include <vector>
#include "Menu.h"


Menu::~Menu() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Menu::init(ShaderProgram& program) {
	
	vector<float> vertices;
	
	mainMenu.loadFromFile("images/menu0.png", TEXTURE_PIXEL_FORMAT_RGBA);
	Lvl1.loadFromFile("images/menu1.png", TEXTURE_PIXEL_FORMAT_RGBA);
	Lvl2.loadFromFile("images/menu2.png", TEXTURE_PIXEL_FORMAT_RGBA);
	Lvl3.loadFromFile("images/menu3.png", TEXTURE_PIXEL_FORMAT_RGBA);
	Lvl4.loadFromFile("images/menu4.png", TEXTURE_PIXEL_FORMAT_RGBA);
	LvlBoss.loadFromFile("images/menu5.png", TEXTURE_PIXEL_FORMAT_RGBA);
	gameOver.loadFromFile("images/menu6.png", TEXTURE_PIXEL_FORMAT_RGBA);
	timeOver.loadFromFile("images/menu9.png", TEXTURE_PIXEL_FORMAT_RGBA);
	instructionsMenu.loadFromFile("images/menu7.png", TEXTURE_PIXEL_FORMAT_RGBA);
	creditsMenu.loadFromFile("images/menu8.png", TEXTURE_PIXEL_FORMAT_RGBA);
	winMenu.loadFromFile("images/menu10.png", TEXTURE_PIXEL_FORMAT_RGBA);

	// First triangle
	vertices.push_back(-1.f); vertices.push_back(-1.f);
	vertices.push_back(0.f); vertices.push_back(1.f);
	vertices.push_back(1.f); vertices.push_back(-1.f);
	vertices.push_back(1.f); vertices.push_back(1.f);
	vertices.push_back(1.f); vertices.push_back(1.f);
	vertices.push_back(1.f); vertices.push_back(0.f);

	// Second triangle
	vertices.push_back(-1.f); vertices.push_back(-1.f);
	vertices.push_back(0.f); vertices.push_back(1.f);
	vertices.push_back(1); vertices.push_back(1);
	vertices.push_back(1.f); vertices.push_back(0.f);
	vertices.push_back(-1.f); vertices.push_back(1.f);
	vertices.push_back(0.f); vertices.push_back(0.f);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Menu::render() {
	glEnable(GL_TEXTURE_2D);
	renderImage.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_TEXTURE_2D);
}

void Menu::setImage(int i) {
	switch (i) {
	case 0: {
		renderImage = mainMenu;
		break;
	}
	case 1: {
		renderImage = Lvl1;
		break;
	}
	case 2: {
		renderImage = Lvl2;
		break;
	}
	case 3: {
		renderImage = Lvl3;
		break;
	}
	case 4: {
		renderImage = Lvl4;
		break;
	}
	case 5: {
		renderImage = LvlBoss;
		break;
	}
	case 6: {
		renderImage = gameOver;
		break;
	}
	case 7: {
		renderImage = instructionsMenu;
		break;
	}
	case 8: {
		renderImage = creditsMenu;
		break;
	}
	case 10: {
		renderImage = winMenu;
		break;
	}
	case 11: {
		renderImage = timeOver;
		break;
	}
	}
}