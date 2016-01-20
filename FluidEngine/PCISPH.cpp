#include "PCISPH.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

PCISPH::PCISPH(vector<Particle>* particles)
{
	this->particles = particles;
	mass = 1.0f;
	smoothingLength = 2.0f;
	stiffness = 1.0f;
	restDensity = 1.0f;
	viscosity = 1.0f;
}

void PCISPH::compute(float dt)
{
	// Ez to read
	float m = mass;
	float h = smoothingLength;
	float k = stiffness;

	for (Particle &p : *particles)
	{
		// Find p's neighbors and add to its list TODO Spacial hash grid
		p.Neighbors.clear();
		for (Particle &pN : *particles)
		{
			if (&p == &pN) break;

			if (distance2(p.Position, pN.Position) < h * h)
			{
				p.Neighbors.push_back(&pN);
			}
		}

		// Compute density and pressure
		for (Particle* pN : p.Neighbors)
		{
			p.Density += m * Poly6(&p, pN, h);
		}
		p.Pressure = k * (p.Density - restDensity);

		// TODO leapfrog integrator
		p.Velocity += vec3(0, -0.3, 0) * dt;
		p.Position += p.Velocity * dt;

		// Collision response
		if (p.Position.y < -5)
		{
			p.Position.y = -5;
			p.Velocity.y = 0;
		}
	}
}

// Smoothing kernels
float PCISPH::Poly6(Particle* p1, Particle* p2, float h)
{
	float coef = 315.0f / (64.0f * PI * pow(h, 9));
	float r2 = distance2(p1->Position, p2->Position);
	return coef * pow(h * h - r2, 3);
}

float PCISPH::Poly6Gradient(Particle* p1, Particle* p2, float h)
{
	float coef = -945.0f / (32.0f * PI * pow(h, 9));
	float r2 = distance2(p1->Position, p2->Position);
	float r = sqrt(r2);
	return coef * r * pow(h * h - r2, 2);
}

float PCISPH::Poly6Laplacian(Particle* p1, Particle* p2, float h)
{
	float coef = -945.0f / (32.0f * PI * pow(h, 9));
	float r2 = distance2(p1->Position, p2->Position);
	return coef * (h * h - r2) * (3 * h * h - 7 * r2);
}