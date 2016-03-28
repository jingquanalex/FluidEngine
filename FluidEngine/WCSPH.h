#pragma once

#include "Includes.h"
#include "Particle.h"
#include "Camera.h"
#include <unordered_map>

typedef std::unordered_multimap<int, Particle*> Grid;

class WCSPH
{

protected:

	const float PI = 3.14159265359f;

	int maxParticles;

	float cellSize;
	int cellCount;
	Grid pGrid;
	glm::ivec3 getCellPos(glm::vec3 position) const;
	int getHashKey(glm::ivec3 cellPos) const;

	std::vector<Particle>* particles;
	float dt;
	float mass;
	float radius;
	float smoothingLength;
	float gasConstant;
	float restDensity;
	float viscosity;
	float surfaceTensionCoef;
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
	float SurfaceTensionC(glm::vec3 pos1, glm::vec3 pos2, float h);

	int pDebugId = -1;
	Camera* camera;

	void resolveCollision(Particle* p);
	void resolveCollision(glm::vec3& position, glm::vec3& velocity);

	bool gravityEnabled = true;

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
	void toggleGravity();
	
};