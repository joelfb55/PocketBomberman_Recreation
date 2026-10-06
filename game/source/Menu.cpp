#include <vector>
#include "Menu.h"


Menu::~Menu() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Menu::init(ShaderProgram& program) {
	
	vector<float> vertices;

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
	image.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_TEXTURE_2D);
}

void Menu::setImage(int i) {
	image.loadFromFile("images/menu" + to_string(i) + ".png", TEXTURE_PIXEL_FORMAT_RGBA);
}