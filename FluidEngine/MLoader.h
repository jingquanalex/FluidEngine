#pragma once

#include "Includes.h"
#include "Particle.h"

class MLoader
{

private:

	std::vector<std::vector<sParticle>> listSParticles;
	int currentListIndex = 0;
	int count;
	GLuint vao, vbo;

public:

	MLoader();
	~MLoader();

	void load();
	void draw();

	std::vector<sParticle>* getParticleData(int index);

};