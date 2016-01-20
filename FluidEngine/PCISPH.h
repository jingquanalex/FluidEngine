#pragma once

#include "Includes.h"
#include "Particle.h"

class PCISPH
{

private:

	std::vector<Particle>* particles;
	float mass;
	float smoothingLength;
	float stiffness;
	float restDensity;
	float viscosity;

	const float PI = glm::pi<float>();
	inline float Poly6(Particle* p1, Particle* p2, float h);
	inline float Poly6Gradient(Particle* p1, Particle* p2, float h);
	inline float Poly6Laplacian(Particle* p1, Particle* p2, float h);

public:

	PCISPH(std::vector<Particle>* particles);

	void compute(float dt);

};