#pragma once

#include "globalIncludes.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"

class Object
{

private:

	// Predefined unit primitives data
	static const GLfloat triVertices[9];
	static const GLuint triIndices[3];
	static const GLfloat cubeVertices[24];
	static const GLfloat cubeTexCoords[24];
	static const GLuint cubeIndices[36];

	glm::mat4 matModel;
	glm::vec3 position;

	Mesh* mesh;
	Shader* shader;

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