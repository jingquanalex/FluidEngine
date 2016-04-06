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
	maxParticles = 5000;
	mass = 28.00f;
	radius = 0.1f;
	smoothingLength = radius * 4;
	restDensity = 429.0f;
	viscosity = 0.00f;
	surfaceTensionCoef = 0.00f;
	gravity = vec3(0, -9.8f, 0);
	densityVariationThreshold = 0.03f * restDensity;

	// Gird
	cellSize = smoothingLength;
	cellCount = 10007;
	pGrid.reserve(cellCount);

	initialize();
}

void PCISPH::initialize()
{
	WCSPH::initialize();

	// === Precompute scaling factor ===
	
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

void PCISPH::compute(ivec2 mouseDelta, int renderMode)
{
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

		p.Color = vec4(0.6, 0.9, 1.0, 1.0);
	}

	// === +Compute Density ===

	for (Particle &p : *particles)
	{
		p.Neighbors.clear();
		p.Density = 0.0f;

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

						// For neighbor particles within smoothing length
						if (distance2(p.Position, pN->Position) <= h * h)
						{
							// === Compute Density ===

							p.Density += m * Poly6(p.Position, pN->Position, h);

							// Push into p's neighbor list
							p.Neighbors.push_back(pN);
						}
					}
				}
			}
		}
	}

	// === Compute Normal (for curvature force) ===

	for (Particle &p : *particles)
	{
		p.Normal = vec3(0);

		for (Particle* pN : p.Neighbors)
		{
			if (pN->Density > 0.0f)
			{
				p.Normal += h * m / pN->Density * Poly6Gradient(p.Position, pN->Position, h);
			}
		}
	}

	// === Compute Non-Pressure Forces ===

	for (Particle &p : *particles)
	{
		fGravity = m * gravity;
		fExternal = vec3(0);
		fViscosity = vec3(0);
		p.Pressure = 0.0f;
		p.PressureForce = vec3(0);
		vec3 fCohesion = vec3(0);
		vec3 fCurvature = vec3(0);
		vec3 fSurfaceTension = vec3(0);

		for (Particle* pN : p.Neighbors)
		{
			if (p.Density == 0.0f || pN->Density == 0.0f) continue;

			// Viscosity
			fViscosity += u * m * (pN->Velocity - p.Velocity) / pN->Density *
				ViscosityLaplacian(p.Position, pN->Position, h);

			// Surface tension
			if (distance2(p.Position, pN->Position) > 0.0f)
			{
				float correctionFactor = 2 * restDensity / (p.Density + pN->Density);
				float cohesionKernel = SurfaceTensionC(p.Position, pN->Position, h);

				fCohesion += -surfaceTensionCoef * m * m * cohesionKernel *
					normalize(p.Position - pN->Position) *
					correctionFactor;

				fCurvature += -surfaceTensionCoef * m * (p.Normal - pN->Normal) *
					correctionFactor;
			}
		}

		fSurfaceTension = fCohesion + fCurvature;

		// External force (click and drag)
		if (pD && distance2(pD->Position, p.Position) < pow(1.0f, 2))
		{
			mat4 mView = camera->getMatView();
			vec3 up = vec3(mView[0].y, mView[1].y, mView[2].y);
			vec3 right = cross(camera->getDirectionVec(), up);
			fExternal = (mouseDelta.x * right + -mouseDelta.y * up) * m * 5;
		}

		p.NonPressureForce = fExternal + fViscosity + fSurfaceTension;
		if (gravityEnabled) p.NonPressureForce += fGravity;
	}

	// === Prediction Correction Loop ===

	int iter = 0;
	//while (maxDensityVariation > densityVariationThreshold || iter++ < 3)
	while (iter++ < 3)
	{
		maxDensityVariation = 0.0f;

		// === Predict velocity and position ===

		for (Particle &p : *particles)
		{
			p.Force = p.PressureForce + p.NonPressureForce;
			vec3 acceleration = p.Force / m;

			p.VelocityPredicted = p.Velocity;
			p.PositionPredicted = p.Position;

			// Semi-implicit Euler
			/*p.VelocityPredicted += acceleration * dt;
			p.PositionPredicted += p.VelocityPredicted * dt;*/

			// Leap frog
			p.VelocityPredicted += 0.5f * (p.OldAccelerationPredicted + acceleration) * dt;
			p.PositionPredicted += p.VelocityPredicted * dt + 0.5f * acceleration * dt * dt;
			p.OldAccelerationPredicted = acceleration;

			resolveCollision(p.PositionPredicted, p.VelocityPredicted);
		}

		// === Predict Density, Density Variation, Pressure ===

		for (Particle &p : *particles)
		{
			p.DensityPredicted = 0.0f;

			// Predict density
			for (Particle* pN : p.Neighbors)
			{
				p.DensityPredicted += m * Poly6(p.PositionPredicted, pN->PositionPredicted, h);
			}

			// Density variation
			//p.DensityVariation = glm::max(0.0f, p.DensityPredicted - restDensity);
			p.DensityVariation = p.DensityPredicted - restDensity;

			// Max density variation form all particles
			if (maxDensityVariation < p.DensityVariation)
			{
				maxDensityVariation = p.DensityVariation;
			}

			// Update pressure
			p.Pressure += scalingFactorDelta * p.DensityVariation;
			//p.Pressure += 0.1 * p.DensityVariation;
		}

		//cout << iter << ": " << maxDensityVariation / restDensity << endl;
		//if (maxDensityVariation / restDensity < densityVariationThreshold) goto endPredictionLoop;
		
		// === Compute Pressure Force ===

		for (Particle &p : *particles)
		{
			for (Particle* pN : p.Neighbors)
			{
				p.PressureForce += -m * m * 
					(p.Pressure / pow(p.DensityPredicted, 2) + pN->Pressure / pow(pN->DensityPredicted, 2)) *
					SpikyGradient(p.PositionPredicted, pN->PositionPredicted, h);
			}
		}
	}

	endPredictionLoop:

	// === Integrate New Velocity and Position ===

	for (Particle &p : *particles)
	{
		p.Force = p.PressureForce + p.NonPressureForce;
		vec3 acceleration = p.Force / m;

		// Leapfrog integration
		p.Velocity += 0.5f * (p.OldAcceleration + acceleration) * dt;
		p.Position += p.Velocity * dt + 0.5f * acceleration * dt * dt;
		p.OldAcceleration = acceleration;

		// Color particles
		if (renderMode == 0)
		{
			p.Color -= vec4(0.02f) * p.Density / restDensity;
			p.Color += vec4(0.1f) * length(p.Velocity) / 2;
		}

		if (p.Id == pDebugId)
		{
			//p.Color = vec4(1, 0, 0, 1);
			for (Particle* pN : p.Neighbors)
			{
				pN->Color = vec4(1, 0, 0, 1);
			}
		}

		resolveCollision(&p);
	}

	// printout debug
	/*float duration = 5.0f;
	if (particles->size() > 0)
	{
		if (elasped < duration)
		{
			if (particles->size() > 1)
			{
				Particle p = particles->at(1);
				fprintf(dFile, "id=%i, dt=%.4f, x=%.4f, y=%.4f, z=%.4f, d=%.4f, p=%.4f, pFx=%.4f, pFy = %.4f, pFz=%.4f \n",
					p.Id, elasped, p.Position.x, p.Position.y, p.Position.z,
					p.Density, p.Pressure, p.PressureForce.x, p.PressureForce.y, p.PressureForce.z);
			}
		}
		else if (elasped > duration)
		{
			fclose(dFile);
		}

		elasped += dt;
	}*/
}