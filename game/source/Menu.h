#ifndef _MENU_INCLUDE
#define _MENU_INCLUDE

#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


class Menu 
{
public:
	~Menu();

	void init(ShaderProgram& program);
	void render();
	void setImage(int i);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	Texture image;
};

#endif