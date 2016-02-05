#pragma once

#include "Includes.h"
#include "Particle.h"
#include <unordered_map>

class WCSPH
{

private:

	struct KeyHash
	{
		size_t operator()(const glm::ivec3& key) const
		{
			return ((key.x * 73856093) ^ (key.y * 19349663) ^ (key.z * 83492791));
		}
	};

	float cellSize;
	std::unordered_multimap<glm::ivec3, Particle*, KeyHash> pGrid;

	glm::ivec3 discretize(glm::vec3 position);

	std::vector<Particle>* particles;
	float mass;
	float smoothingLength;
	float gasConstant;
	float restDensity;
	float viscosity;
	glm::vec3 oldAcceleration;
	glm::vec3 gravity;

	const float PI = 3.14159265359f;
	inline float Poly6(Particle* p1, Particle* p2, float h);
	inline glm::vec3 Poly6Gradient(Particle* p1, Particle* p2, float h);
	inline float Poly6Laplacian(Particle* p1, Particle* p2, float h);
	inline glm::vec3 SpikyGradient(Particle* p1, Particle* p2, float h);
	inline float ViscosityLaplacian(Particle* p1, Particle* p2, float h);

	Particle* pD = nullptr;

public:

	WCSPH(std::vector<Particle>* particles);

	void compute(float dt);

	void clear();
	void setDebugParticle(Particle* p);
	float getSmoothingLength() const;
	
};