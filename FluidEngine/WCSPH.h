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

	float cellSize;
	Grid pGrid;

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
	inline float Poly6(glm::vec3 pos1, glm::vec3 pos2, float h);
	inline glm::vec3 Poly6Gradient(glm::vec3 pos1, glm::vec3 pos2, float h);
	inline float Poly6Laplacian(glm::vec3 pos1, glm::vec3 pos2, float h);
	inline glm::vec3 SpikyGradient(glm::vec3 pos1, glm::vec3 pos2, float h);
	inline float ViscosityLaplacian(glm::vec3 pos1, glm::vec3 pos2, float h);

	int pDebugId = -1;
	Camera* camera;

public:

	WCSPH(std::vector<Particle>* particles, Camera* camera);

	void compute(float dt, glm::ivec2 mouseDelta);

	void clear();
	void setDebugParticle(int id);
	float getSmoothingLength() const;
	int getParticleAtRay(glm::vec3 ray) const;
	bool isIntersectingRaySphere(glm::vec3 ray, glm::vec3 spherePos, float radius) const;
	
};