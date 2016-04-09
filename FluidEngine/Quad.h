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
	std::vector<GLuint> listTexCubeId;
	int currentTexIndex = 0;

public:

	Quad(glm::vec3 position = glm::vec3());
	~Quad();

	void load(std::string shadername = "quad");
	void load(std::string mapname, std::string shadername);
	void loadCubemap(std::string mapname);
	void draw();
	void draw(GLuint depthMapId, Camera* camera = nullptr);
	
	void cycleNextMap();
	void cyclePreviousMap();

	GLuint getCubeMap() const;
	GLuint getVao() const;

};