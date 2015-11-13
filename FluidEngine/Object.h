#pragma once

#include "Includes.h"
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

protected:

	glm::mat4 matModel;
	glm::vec3 position, rotation, scale;
	glm::vec3 color;

	Model* model;
	Shader* shader;

public:

	Object(glm::vec3 position = glm::vec3());
	~Object();

	void load(std::string modelname, std::string shadername = "basic");
	virtual void update(float dt);
	virtual void draw();

	// === Accessors ===

	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);

	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;
	glm::vec3 getScale() const;
};