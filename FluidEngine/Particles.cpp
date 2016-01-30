#include "Particles.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Particles::Particles()
{
	count = 0;
	maxCount = 0;
}

Particles::~Particles()
{

}

void Particles::load(int maxParticleCount)
{
	// Set max particles count and load particle shaders
	maxCount = maxParticleCount;
	shader = new Shader("particle");
	solver = new PCISPH(&particles);

	// Load a box model to contain the particles
	box = new Object();
	box->setScale(vec3(10));
	box->setWireframeMode(true);
	box->load("cube.obj");

	// Add some particles temp
	addParticles(100);

	// Initialize vertex buffer memory and vao
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, maxCount * sizeof(sParticle), NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle),
		(GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(sParticle),
		(GLvoid*)offsetof(sParticle, sParticle::Color));

	glBindVertexArray(0);

	glUseProgram(shader->getProgram());
	glUniform1f(glGetUniformLocation(shader->getProgram(), "radius"), 0.1f);
	glUseProgram(0);
}

// Update particle positions and send to vbo
void Particles::update(float dt)
{
	solver->compute(dt);
	
	// Remove stray particles
	auto it = particles.begin();
	while(it != particles.end())
	{
		if (length(it->Position) > 10)
		{
			it = particles.erase(remove(particles.begin(), particles.end(), *it), particles.end());
		}
		else
		{
			it++;
		}
	}

	// Create particle data list to send to gpu
	sParticles.clear();
	for (Particle &p : particles)
	{
		sParticle sp;
		sp.Position = p.Position;
		sp.Color = p.Color;
		sParticles.push_back(sp);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(sParticle), sParticles.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Render particles
void Particles::draw()
{
	glUseProgram(shader->getProgram());
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glBindVertexArray(0);
	glUseProgram(0);

	box->draw();
}

float Particles::fRandom(float low, float high)
{
	return low + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (high - low)));
}

// Add a specified number of Particles with a random position
void Particles::addParticles(int value)
{
	for (int i = 0; i < value; i++)
	{
		Particle particle;
		particle.Position.x = fRandom(-2.5f, 2.5f);
		particle.Position.y = fRandom(-2.5f, 2.5f);
		particle.Position.z = fRandom(-2.5f, 2.5f);
		particle.Color = vec4(1.0);
		particles.push_back(particle);
	}
	count += value;
}

// Remove a specified number of Particles
void Particles::removeParticles(int value)
{
	if (value < count)
	{
		particles.resize(count - value);
		count -= value;
	}
	else
	{
		particles.clear();
		count = 0;
	}
}

void Particles::keyboard(unsigned char key)
{
	switch (key)
	{
	case ' ':
		addParticles(100);
		break;
	case 'c':
		removeParticles(count);
		break;
	}
}