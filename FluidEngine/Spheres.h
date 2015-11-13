#pragma once

#include "Includes.h"
#include <vector>
#include "Object.h"

// Render 2D spheres on quads generated from a set of points

class Spheres : public Object
{

private:

	struct Sphere
	{
		glm::vec3 position;
	};

	GLuint vao, vbo;
	std::vector<Sphere> spheres;
	GLuint texid;

public:

	Spheres();
	~Spheres();

	void load();
	void update(float dt);
	void draw();

};