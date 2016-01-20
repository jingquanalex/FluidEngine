#pragma once

#include "Includes.h"
#include "Object.h"
#include "PCISPH.h"

// Particle Manager: Generate, solve and render particles
// Particles are rendered on 2D quads created with geometry shader

class Particles : public Object
{

private:

	std::vector<Particle> particles;
	int count, maxCount;
	PCISPH* solver;

	GLuint vao, vbo;
	GLuint texid;

	Object* box;

public:

	Particles();
	~Particles();

	void load(int maxParticleCount = 1000);
	void update(float dt);
	void draw();

	void addParticles(int value);
	void removeParticles(int value);

};