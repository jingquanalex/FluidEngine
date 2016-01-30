#include "PCISPH.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

PCISPH::PCISPH(vector<Particle>* particles)
{
	this->particles = particles;
	oldAcceleration = vec3(0);

	// === Variables ===
	mass = 0.01f;
	smoothingLength = 0.5f;
	gasConstant = 3.1f;
	restDensity = 1.0f;
	viscosity = 3.0f;
	cellSize = (int)floor(smoothingLength * 10);
}

void PCISPH::compute(float dt)
{
	// Short variables
	int n = particles->size();
	float m = mass;
	float h = smoothingLength;
	float k = gasConstant;
	float u = viscosity;

	// === Neighbor Search ===
	

	for (Particle &p : *particles)
	{
		p.Neighbors.clear();

		for (Particle &pN : *particles)
		{
			// Include only neighbors of p
			if (&p == &pN) break;

			// if its distance from p is <= h
			if (distance2(p.Position, pN.Position) <= h * h)
			{
				p.Neighbors.push_back(&pN);
			}

			// === Compute Density ===
			p.Density += m * Poly6(&p, &pN, h);
		}

		p.Pressure = k * (p.Density - restDensity);

		if (abs(p.Pressure) > 10)
		{
			p.Pressure = p.Pressure;
		}
	}

	// === Compute Forces ===
	for (Particle &p : *particles)
	{
		vec3 fPressure = vec3(0);
		vec3 fViscosity = vec3(0);
		vec3 fGravity = m * vec3(0, -0.3, 0);

		for (Particle* pN : p.Neighbors)
		{
			fPressure += m * (p.Pressure + pN->Pressure) / (2 * pN->Density) * 
				SpikyGradient(&p, pN, h);

			fViscosity += m * (pN->Velocity - p.Velocity) / pN->Density *
				ViscosityLaplacian(&p, pN, h);
		}
		fViscosity *= u;

		p.Force = fPressure + fViscosity + fGravity;
	}

	// === Compute New Velocity and Position ===
	for (Particle &p : *particles)
	{
		// Leapfrog integration
		vec3 acc = p.Force / m;
		p.Position += p.Velocity * dt + 0.5f * acc * dt * dt;
		p.Velocity += 0.5f * (oldAcceleration + acc) * dt;
		oldAcceleration = acc;

		// Collision response
		if (p.Position.y < -5)
		{
			p.Position.y = -5;
			p.Velocity.y = -p.Velocity.y/2;
		}
	}
}

// === Smoothing Kernels ===
// Poly6 used for everything except in pressure and viscosity
float PCISPH::Poly6(Particle* p1, Particle* p2, float h)
{
	float coef = 315.0f / (64.0f * PI * pow(h, 9));
	float r2 = distance2(p1->Position, p2->Position);
	return coef * pow(h * h - r2, 3);
}

vec3 PCISPH::Poly6Gradient(Particle* p1, Particle* p2, float h)
{
	float coef = 945.0f / (32.0f * PI * pow(h, 9));
	float r2 = distance2(p1->Position, p2->Position);
	vec3 r = p1->Position - p2->Position;
	return -coef * r * pow(h * h - r2, 2);
}

float PCISPH::Poly6Laplacian(Particle* p1, Particle* p2, float h)
{
	float coef = 945.0f / (32.0f * PI * pow(h, 9));
	float r2 = distance2(p1->Position, p2->Position);
	return -coef * (h * h - r2) * (3 * h * h - 7 * r2);
}

// To calculate pressure force
inline vec3 PCISPH::SpikyGradient(Particle* p1, Particle* p2, float h)
{
	float coef = 45.0f / (PI * pow(h, 6));
	vec3 r = p1->Position - p2->Position;
	return -coef * normalize(r) * pow(h - length(r), 2);
}

// For viscosity force
inline float PCISPH::ViscosityLaplacian(Particle* p1, Particle* p2, float h)
{
	float coef = 45.0f / (PI * pow(h, 6));
	return coef * (h - glm::distance(p1->Position, p2->Position));
}

float PCISPH::getSmoothingLength() const
{
	return smoothingLength;
}