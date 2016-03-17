#include "PCISPH.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

// === This implementation is based on Predictive Corrective Imcompressible SPH... ===
// https://graphics.ethz.ch/~sobarbar/papers/Sol09/Sol09.pdf

PCISPH::PCISPH(float dt, vector<Particle>* particles, Camera* camera) : WCSPH(dt, particles, camera)
{
	dFile = fopen("ddebug.txt", "w");

	// === User Constants ===

	// Particles
	/*radius = 0.019444f;
	restDensity = 999.927f;
	viscosity = 0.13f;
	mass = restDensity / pow(1.0f / (2.0f * radius), 3);*/
	radius = 0.1f;
	restDensity = 34.0f;
	viscosity = 0.13f;
	mass = 1.00f;
	smoothingLength = radius * 4;
	gravity = vec3(0, -9.8, 0);
	densityVariationThreshold = restDensity * 3 / 100;

	// Gird
	cellSize = smoothingLength;

	// === Precompute Constants ===

	initialize();
}

void PCISPH::initialize()
{
	WCSPH::initialize();

	// Precompute scaling factor kernel equation
	
	vec3 sumW = vec3(0);
	float sumWdotW = 0;

	// Assume particles are tightly packed
	float tWidth = smoothingLength + 2 * radius;
	for (float z = -tWidth; z <= tWidth; z += 2 * radius)
	{
		for (float y = -tWidth; y <= tWidth; y += 2 * radius)
		{
			for (float x = -tWidth; x <= tWidth; x += 2 * radius)
			{
				vec3 nPos = vec3(x, y, z);
				if (dot(nPos, nPos) < smoothingLength * smoothingLength)
				{
					vec3 dW = Poly6Gradient(nPos, vec3(0), smoothingLength);
					sumW += dW;
					sumWdotW += dot(dW, dW);
				}
			}
		}
	}

	float beta = dt * dt * mass * mass * 2 / pow(restDensity, 2);
	scalingFactorDelta = -1.0f / (beta * (-dot(sumW, sumW) - sumWdotW));
}

void PCISPH::compute(ivec2 mouseDelta)
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
		pGrid.insert(make_pair(getHashKey(getCellPos(p.Position)), &p));

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
					auto itb = pGrid.equal_range(getHashKey(getCellPos(samplePos)));
					for (auto it = itb.first; it != itb.second; it++)
					{
						Particle* pN = it->second;

						// Push into p's neighbor list if within smoothing length
						// Include p in the list as well
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

	// === Compute Non-Pressure Forces ===

	for (Particle &p : *particles)
	{
		fViscosity = vec3(0);
		fGravity = gravity; // m * gravity
		fExternal = vec3(0);
		p.Pressure = 0.0f;
		p.PressureForce = vec3(0);
		p.VelocityPredicted = p.Velocity;
		p.PositionPredicted = p.Position;

		// Density (include p)
		for (Particle &p : *particles)
		{
			p.Density = 0.0f;

			for (Particle* pN : p.Neighbors)
			{
				p.Density += m * Poly6(p.PositionPredicted, pN->PositionPredicted, h);
			}
		}
		
		// Viscosity force
		for (Particle* pN : p.Neighbors)
		{
			if (pN->Density > 0.00001f)
			{
				fViscosity += m * (pN->Velocity - p.Velocity) / pN->Density *
					ViscosityLaplacian(p.Position, pN->Position, h);
			}
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
	}

	// === Prediction Correction Scheme ===

	float maxDensityVariation = 0.0f;
	int iter = 0;
	bool isrun = true;
 	while (isrun && (maxDensityVariation > densityVariationThreshold || iter < 3))
	{
		maxDensityVariation = 0.0f;

		// === Predict velocity and position ===

		for (Particle &p : *particles)
		{
			p.Force += p.PressureForce;

			// Semi-implicit Euler
			//p.VelocityPredicted += p.Force / m * dt;
			//p.PositionPredicted += p.VelocityPredicted * dt;

			// Leap frog
			vec3 acceleration = p.Force / m;
			p.VelocityPredicted += 0.5f * (p.OldAccelerationPredicted + acceleration) * dt;
			p.PositionPredicted += p.VelocityPredicted * dt + 0.5f * acceleration * dt * dt;
			p.OldAccelerationPredicted = acceleration;

			resolveCollision(p.PositionPredicted, p.VelocityPredicted);
		}

		// === Predict Density, Density Variation, Pressure ===

		for (Particle &p : *particles)
		{
			p.DensityPredicted = 0.0f;

			for (Particle* pN : p.Neighbors)
			{
				// Predict density
				if (&p != pN && distance2(p.Position, pN->Position) < 2.0f)
				{
					p.Position = p.Position;
				}
				p.DensityPredicted += m * Poly6(p.PositionPredicted, pN->PositionPredicted, h);
			}

			// Density variation
			p.DensityVariation = p.DensityPredicted - restDensity;
			if (maxDensityVariation < p.DensityVariation)
			{
				maxDensityVariation = p.DensityVariation;
			}

			//DEBUG
			/*if (iter > 100)
			{
				p.Position = p.Position;
				//isrun = false;
			}*/

			// Update pressure
			p.Pressure += scalingFactorDelta * p.DensityVariation;
		}

		// === Compute Pressure Force ===

		for (Particle &p : *particles)
		{
			for (Particle* pN : p.Neighbors)
			{
				if (p.Id != pN->Id)
				{
					p.PressureForce = -m * m * 
						(p.Pressure / pow(p.Density, 2) + pN->Pressure / pow(pN->Density, 2)) *
						SpikyGradient(p.PositionPredicted, pN->PositionPredicted, h);
				}
			}
		}

		iter++;
		//break;
	}

	// === Integrate New Velocity and Position ===

	for (Particle &p : *particles)
	{
		// Leapfrog integration
		vec3 acceleration = p.Force / m;
		p.Velocity += 0.5f * (p.OldAcceleration + acceleration) * dt;
		p.Position += p.Velocity * dt + 0.5f * acceleration * dt * dt;
		p.OldAcceleration = acceleration;

		// Color particles
		if (p.Id != pDebugId)
		{
			p.Color = vec4(1);
		}
		else
		{
			p.Color = vec4(1, 0, 0, 1);
		}

		resolveCollision(&p);
	}

	// printout debug
	/*if (particles->size() > 0)
	{
		if (elasped < duration)
		{
			Particle p = particles->at(1);
			fprintf(dFile, "id=%i, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f \n",
				p.Id, elasped, p.Position.x, p.Position.y, p.Position.z, 
				p.Density, p.Pressure, p.PressureForce);
		}
		else if (elasped > duration)
		{
			fclose(dFile);
		}

		elasped += dt;
	}*/
}