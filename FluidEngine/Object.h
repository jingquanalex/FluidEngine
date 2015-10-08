#pragma once

#include "globalIncludes.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "Camera.h"

class Object
{

private:

	// Predefined unit primitives
	static const GLfloat triVertices[9];
	static const GLuint triIndices[3];
	static const GLfloat cubeVertices[24];
	static const GLuint cubeIndices[36];
	static const GLfloat cubeColors[24];

	std::string shadername;
	glm::vec3 position;
	GLuint program;
	GLuint locationMVP;
	GLuint vbo;
	GLuint vao;
	GLuint ebo;

	Camera* camera;
	
	// === Private Functions ===

	std::string readFile(std::string filename);
	GLuint makeProgram(std::string name);

public:

	Object(Camera* camera);
	~Object();

	void load(std::string shadername = "basic");
	void update(float dt);
	void draw();

	// === Accessors ===

	void setPosition(glm::vec3 position);
	glm::vec3 getPosition() const;
};