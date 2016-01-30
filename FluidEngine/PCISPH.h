#pragma once

#include "Includes.h"
#include "Particle.h"
#include <map>

class PCISPH
{

private:

	struct KeyHash
	{
		size_t operator()(const glm::ivec3& key, int n) const
		{
			return ((key.x * 73856093) ^ (key.y * 19349663) ^ (key.z * 83492791)) % n;
		}
	};

	int cellSize;
	std::multimap<glm::ivec3, Particle*, KeyHash> pGrid;
	std::vector<Particle>* particles;
	float mass;
	float smoothingLength;
	float gasConstant;
	float restDensity;
	float viscosity;
	glm::vec3 oldAcceleration;

	const float PI = glm::pi<float>();
	inline float Poly6(Particle* p1, Particle* p2, float h);
	inline glm::vec3 Poly6Gradient(Particle* p1, Particle* p2, float h);
	inline float Poly6Laplacian(Particle* p1, Particle* p2, float h);
	inline glm::vec3 SpikyGradient(Particle* p1, Particle* p2, float h);
	inline float ViscosityLaplacian(Particle* p1, Particle* p2, float h);

public:

	PCISPH(std::vector<Particle>* particles);

	void compute(float dt);

	float getSmoothingLength() const;

};