#pragma once

#include "globalIncludes.h"
#include <vector>
#include "Object.h"

class Spheres : public Object
{

private:

	struct Sphere
	{
		glm::vec3 position;
		GLfloat radius;
	};

	GLuint vao, vbo;
	std::vector<Sphere> spheres;

public:

	Spheres();
	~Spheres();

	void load();
	void update(float dt, Camera* camera);
	void draw();

};