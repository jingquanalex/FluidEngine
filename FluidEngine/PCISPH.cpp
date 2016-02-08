#include "PCISPH.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

// === This implementation is based on Predictive Corrective Imcompressible SPH... ===
// https://graphics.ethz.ch/~sobarbar/papers/Sol09/Sol09.pdf

PCISPH::PCISPH(vector<Particle>* particles, Camera* camera) : WCSPH(particles, camera)
{
	this->camera = camera;
	this->particles = particles;
	oldAcceleration = vec3(0);
	pGrid.reserve(particles->size() * 2);

	// === User Constants ===

	// Particles
	mass = 1.00f;
	smoothingLength = 0.5f;
	gasConstant = 0.11f;
	restDensity = 24.00f;
	viscosity = 0.13f;
	gravity = vec3(0, -9.8, 0);
	densityVariationThreshold = 0.01f;
	scalingFactor = 1.0f;

	// Gird
	cellSize = smoothingLength;
}

void PCISPH::compute(float dt, ivec2 mouseDelta)
{
	// Shorten variables
	int n = particles->size();
	float m = mass;
	float h = smoothingLength;
	float k = gasConstant;
	float u = viscosity;
	Particle* pD = nullptr;

	// === Neighbor Search ===

	pGrid.clear();
	for (Particle &p : *particles)
	{
		// Insert particles into cells based on its positional key
		pGrid.insert(make_pair(discretize(p.Position), &p));

		if (p.Id == pDebugId)
		{
			pD = &p;
		}
	}

	for (Particle &p : *particles)
	{
		p.Neighbors.clear();

		// Search adjacent cells of the cell at position p
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				for (int k = -1; k <= 1; k++)
				{
					// Sample particles in current cell
					vec3 samplePos = p.Position + vec3(i, j, k) * h;
					auto itb = pGrid.equal_range(discretize(samplePos));
					for (auto it = itb.first; it != itb.second; it++)
					{
						Particle* pN = it->second;

						// Push into p's neighbor list if within smoothing length
						float r2 = distance2(p.Position, pN->Position);
						if (!isinf(r2) && !isnan(r2) && r2 <= h * h)
						{
							p.Neighbors.push_back(pN);
						}
					}
				}
			}
		}
	}

	// === Compute Non-Pressure Forces, Predict Pressure ===

	for (Particle &p : *particles)
	{
		fViscosity = vec3(0);
		fGravity = m * gravity;
		fExternal = vec3(0);
		fPressure = vec3(0);
		p.Pressure = 0.0f;
		
		// Viscosity force
		for (Particle* pN : p.Neighbors)
		{
			if (pN->Density == 0.0f) continue;
			fViscosity += m * (pN->Velocity - p.Velocity) / pN->Density *
				ViscosityLaplacian(p.Position, pN->Position, h);
		}
		fViscosity *= u;

		// External force (click and drag)
		if (pD && distance2(pD->Position, p.Position) < pow(1.0f, 2))
		{
			mat4 mView = camera->getMatView();
			vec3 up = vec3(mView[0].y, mView[1].y, mView[2].y);
			vec3 right = cross(camera->getDirectionVec(), up);
			fExternal = (mouseDelta.x * right + -mouseDelta.y * up) * 5;
		}

		p.Force = fViscosity + fGravity + fExternal;

		// === Prediction Correction Scheme ===

		float densityVariation = 0.0f;
		while (densityVariation < densityVariationThreshold)
		{
			// === Predict velocity and position ===
			
			for (Particle &p : *particles)
			{
				// Semi-implicit Euler
				p.VelocityPredicted += p.Force / m * dt;
				p.PositionPredicted += p.VelocityPredicted * dt;
			}

			// === Predict Density, Density Variation, Pressure ===

			for (Particle &p : *particles)
			{
				// Density
				for (Particle* pN : p.Neighbors)
				{
					p.DensityPredicted += m * Poly6(p.PositionPredicted, pN->PositionPredicted, h);
				}

				// Density variation
				densityVariation = p.DensityPredicted - restDensity;

				// Pressure
				scalingFactor = pow(restDensity, 2) / (2 * dt * dt * m * m * 1000000); //TODO
				p.Pressure += scalingFactor * densityVariation;
			}

			// === Compute Pressure Force ===

			for (Particle* pN : p.Neighbors)
			{
				// Don't compute itself
				if (p.Id != pN->Id)
				{
					fPressure += m * (p.Pressure + pN->Pressure) / (2.0 * pN->Density) *
						SpikyGradient(p.PositionPredicted, pN->PositionPredicted, h);
				}
			}
		}
	}

	// === Integrate New Velocity and Position ===

	for (Particle &p : *particles)
	{
		// Compute total force
		p.Force = fPressure + fViscosity + fGravity + fExternal;

		// Leapfrog integration
		p.Acceleration = p.Force / m;
		p.Velocity += 0.5f * (p.OldAcceleration + p.Acceleration) * dt;
		p.Position += p.Velocity * dt + 0.5f * p.Acceleration * dt * dt;
		p.OldAcceleration = p.Acceleration;

		// Color particles
		if (p.Id != pDebugId)
		{
			p.Color = vec4(1);
		}
		else
		{
			p.Color = vec4(1, 0, 0, 1);
		}

		// TODO Proper Collision response
		if (p.Position.y < -5)
		{
			p.Position.y = -5;
			p.Velocity.y = -p.Velocity.y / 2;
		}

		if (p.Position.x < -5)
		{
			p.Position.x = -5;
			p.Velocity.x = -p.Velocity.x / 2;
		}
		else if (p.Position.x > 5)
		{
			p.Position.x = 5;
			p.Velocity.x = -p.Velocity.x / 2;
		}

		if (p.Position.z < -5)
		{
			p.Position.z = -5;
			p.Velocity.z = -p.Velocity.z / 2;
		}
		else if (p.Position.z > 5)
		{
			p.Position.z = 5;
			p.Velocity.z = -p.Velocity.z / 2;
		}
	}

	// Color debug particle's neighbors
	/*if (pD != nullptr)
	{
	for (Particle* p : pD->Neighbors)
	{
	p->Color = vec4(0, 1, 0, 1);
	}
	}*/
}