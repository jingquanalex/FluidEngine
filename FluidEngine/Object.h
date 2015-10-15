#pragma once

#include "globalIncludes.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Camera.h"

class Object
{

	static std::vector<std::pair<GLuint, std::string>> vecShaders;
	static std::vector<std::pair<GLuint, std::string>>::iterator it;

private:

	// Predefined unit primitives
	static const GLfloat triVertices[9];
	static const GLuint triIndices[3];
	static const GLfloat cubeVertices[24];
	static const GLuint cubeIndices[36];
	static const GLfloat cubeColors[24];

	glm::mat4 matModel, matMVP;
	glm::vec3 position;
	std::string shadername;
	GLuint program;
	GLuint locationMVP;
	GLuint vbo;
	GLuint vao;
	GLuint ebo;
	
	// === Private Functions ===

	std::string readFile(std::string filename);
	GLuint makeProgram(std::string name);

public:

	Object(glm::vec3 position = glm::vec3());
	~Object();

	void load(std::string shadername = "basic");
	void update(float dt, Camera* camera);
	void draw();

	// === Accessors ===

	void setPosition(glm::vec3 position);
	glm::vec3 getPosition() const;
};