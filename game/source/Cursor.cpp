#include <vector>
#include <iostream>
#include "Cursor.h"

using namespace std;

Cursor::Cursor() {

}

Cursor::~Cursor() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Cursor::init() {

	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/simple.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/simple.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	simple.init();
	simple.addShader(vShader);
	simple.addShader(fShader);
	simple.link();
	if (!simple.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << simple.log() << endl << endl;
	}
	simple.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();


	vector<float> vertices;

	// First triangle
	vertices.push_back(-1.f); vertices.push_back(-1.f);
	vertices.push_back(0.f); vertices.push_back(0.f);
	vertices.push_back(-1.f); vertices.push_back(1.f);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = simple.bindVertexAttribute("position", 2, 2 * sizeof(float), 0);
}

void Cursor::render() {
	simple.use();
	simple.setUniformMatrix4f("projection", glm::mat4(1.0f));
	simple.setUniform4f("color", 0.0f, 0.0f, 0.0f, 1.0f);
	simple.setUniformMatrix4f("modelview", glm::mat4(1.0f));
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}