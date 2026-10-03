#ifndef _MENU_INCLUDE
#define _MENU_INCLUDE

#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


class Menu 
{
public:
	Menu(int i, ShaderProgram& program);
	~Menu();

	void render();

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	Texture image;
};

#endif