#pragma once

#include "globalIncludes.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Camera.h"
#include "Model.h"
#include "Shader.h"

class Object
{

private:

	glm::mat4 matModel;
	glm::vec3 position;

	Model* model;
	Shader* shader;

public:

	Object(glm::vec3 position = glm::vec3());
	~Object();

	void load(std::string modelname = "!cube", std::string shadername = "basic");
	void update(float dt, Camera* camera);
	void draw();

	// === Accessors ===

	void setPosition(glm::vec3 position);
	glm::vec3 getPosition() const;
};