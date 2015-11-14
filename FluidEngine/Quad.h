#pragma once

#include "Includes.h"
#include "Object.h"

class Quad : public Object
{

private:

	Mesh* quad;

public:

	Quad(glm::vec3 position = glm::vec3());
	~Quad();

	void load(std::string shadername);
	void draw();

};