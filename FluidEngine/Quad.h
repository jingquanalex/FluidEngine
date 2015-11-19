#pragma once

#include "Includes.h"
#include "Object.h"

class Quad : public Object
{

private:

	GLuint texid;
	GLuint vbo;
	GLuint vao;

public:

	Quad(glm::vec3 position = glm::vec3());
	~Quad();

	void load(std::string mapname, std::string shadername = "quad");
	void draw();

	GLuint getCubeMapID() const;

};