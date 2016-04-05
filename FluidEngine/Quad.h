#pragma once

#include "Includes.h"
#include "Object.h"
#include "Light.h"

class Quad : public Object
{

private:

	GLuint texCubeId;
	GLuint vbo;
	GLuint vao;

public:

	Quad(glm::vec3 position = glm::vec3());
	~Quad();

	void load(std::string mapname, std::string shadername);
	void load(std::string shadername = "quad");
	void draw();
	void draw(GLuint depthMapId, Camera* camera = nullptr);

	GLuint getCubeMap() const;
	GLuint getVao() const;

};