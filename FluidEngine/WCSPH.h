#pragma once

#include "Includes.h"
#include "Particle.h"
#include "Camera.h"
#include <unordered_map>

struct KeyHashS
{
	size_t operator()(const glm::ivec3& key) const
	{
		return ((key.x * 73856093) ^ (key.y * 19349663) ^ (key.z * 83492791));
	}
};

typedef std::unordered_multimap<glm::ivec3, Particle*, KeyHashS> Grid;

class WCSPH
{

protected:

	const float PI = 3.14159265359f;

	float cellSize;
	Grid pGrid;

	glm::ivec3 discretize(glm::vec3 position);

	std::vector<Particle>* particles;
	float dt;
	float mass;
	float radius;
	float smoothingLength;
	float gasConstant;
	float restDensity;
	float viscosity;
	glm::vec3 oldAcceleration;
	glm::vec3 gravity;

	// Shorten variables
	int   n; // Num of particles
	float m; // Mass
	float h; // Smoothing kernel radius
	float k; // Gas constant
	float u; // Viscosity

	/*float poly6Norm;
	float poly6GradientNorm;
	float spikyGradientNorm;
	float viscosityLaplacianNorm;*/
	float Poly6(glm::vec3 pos1, glm::vec3 pos2, float h);
	glm::vec3 Poly6Gradient(glm::vec3 pos1, glm::vec3 pos2, float h);
	glm::vec3 SpikyGradient(glm::vec3 pos1, glm::vec3 pos2, float h);
	float ViscosityLaplacian(glm::vec3 pos1, glm::vec3 pos2, float h);

	int pDebugId = -1;
	Camera* camera;

	void resolveCollision(Particle* p);
	void resolveCollision(glm::vec3& position, glm::vec3& velocity);

public:

	WCSPH(float dt, std::vector<Particle>* particles, Camera* camera);

	virtual void initialize();
	virtual void compute(glm::ivec2 mouseDelta);

	void clear();
	void setDebugParticle(int id);
	float getRadius() const;
	float getSmoothingLength() const;
	int getParticleAtRay(glm::vec3 ray) const;
	bool isIntersectingRaySphere(glm::vec3 ray, glm::vec3 spherePos, float radius) const;
	
};