#pragma once

#include "Includes.h"
#include "Object.h"
//#include "WCSPH.h"
#include "PCISPH.h"

// Particle Manager: Generate, solve and render particles
// Particles are rendered on 2D quads created with geometry shader

class Particles : public Object
{

private:

	// Particle data to upload to GPU
	struct sParticle
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	std::vector<Particle> particles;
	std::vector<sParticle> sParticles;
	int count, maxCount;
	float particleRadius;
	PCISPH* solver;

	GLuint vao, vbo;
	GLuint texid;

	Object* box;

	bool statePicked;
	int mouseX, mouseY;
	int mouseLastX = 0, mouseLastY = 0;
	glm::ivec2 mouseDelta;
	Camera* camera;

public:

	Particles();
	~Particles();

	void load(Camera* camera);
	void update(float dt);
	void draw();

	float fRandom(float low, float high);
	void addParticles(int value);
	void removeParticles(int value);

	void mouse(int button, int state);
	void mouseMove(int x, int y);
	void mouseMovePassive(int x, int y);
	void keyboard(unsigned char key);

};