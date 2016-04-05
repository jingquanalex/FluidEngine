#include "WCSPH.h"

using namespace glm;
using namespace std;
using namespace concurrency;

extern string g_mediaDirectory;

// === This implementation is loosely based on Weakly Compressable SPH... ===
// http://cg.informatik.uni-freiburg.de/publications/2007_SCA_SPH.pdf

WCSPH::WCSPH(float dt, vector<Particle>* particles, Camera* camera)
{
	this->dt = dt;
	this->particles = particles;
	this->camera = camera;
	oldAcceleration = vec3(0);
	
	// === Variables ===

	/*
	* surface tension is dependent of mass > 10
	* determine mass first
	* then find balance with gasconstant and restdensity
	* higher the gasconstant the easier particle swirl
	* viscosity > 0.8 for stable simulation at current mass
	*/

	// Particles
	maxParticles = 5000;
	mass = 28.00f;
	radius = 0.1f;
	smoothingLength = radius * 4;
	restDensity = 59.0f;
	viscosity = 3.8f;
	surfaceTensionCoef = 1.0f;
	gasConstant = 0.0000001f;
	gravity = vec3(0, -9.8f, 0);

	/*maxParticles = 5000;
	mass = 100.00f;
	radius = 0.1f;
	smoothingLength = radius * 4;
	restDensity = 999.0f;
	viscosity = 2.8f;
	surfaceTensionCoef = 0.2f;
	gasConstant = 0.01f;
	gravity = vec3(0, -9.8f, 0);*/

	// Gird
	cellSize = smoothingLength;
	cellCount = 10007;
	pGrid.reserve(cellCount);

	initialize();
}

void WCSPH::initialize()
{
	// Shorten variables
	n = particles->size();
	m = mass;
	h = smoothingLength;
	k = gasConstant;
	u = viscosity;
}

// === Hash ===

glm::ivec3 WCSPH::getCellPos(glm::vec3 position) const
{
	return ivec3(
		(int)floor(position.x / cellSize),
		(int)floor(position.y / cellSize),
		(int)floor(position.z / cellSize));
}

int WCSPH::getHashKey(ivec3 cellPos) const
{
	return ((cellPos.x * 73856093) ^ (cellPos.y * 19349663) ^ (cellPos.z * 83492791));// % cellCount;
}

void WCSPH::compute(ivec2 mouseDelta)
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

		// Default color
		p.Color = vec4(0.6, 0.9, 1.0, 1.0);
	}

	// === +Compute Density ===

	for (Particle &p : *particles)
	//for_each(particles->begin(), particles->end(), [&](Particle& p)
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

	// === Compute Normal And Pressure ===

	for (Particle &p : *particles)
	{
		// === Normal (for curvature force) ===

		p.Normal = vec3(0);

		for (Particle* pN : p.Neighbors)
		{
			if (pN->Density > 0.0f)
			{
				p.Normal += h * m / pN->Density * Poly6Gradient(p.Position, pN->Position, h);
			}
		}

		// === Pressure ===

		// Ideal gas law (Muller et al 2003)
		//p.Pressure = k * (p.Density - restDensity);

		// Tait equation (Becker & Teschner 2007)
		p.Pressure = k * (pow(p.Density / restDensity, 7) - 1);
	}

	// === Compute Forces ===

	for (Particle &p : *particles)
	{
		vec3 fPressure = vec3(0);
		vec3 fViscosity = vec3(0);
		vec3 fGravity = m * gravity;
		vec3 fExternal = vec3(0);
		vec3 fCohesion = vec3(0);
		vec3 fCurvature = vec3(0);
		vec3 fSurfaceTension = vec3(0);

		for (Particle* pN : p.Neighbors)
		{
			if (p.Density == 0.0f || pN->Density == 0.0f) continue;

			/*fPressure += -m * (p.Pressure + pN->Pressure) / (2.0 * pN->Density) *
				SpikyGradient(p.Position, pN->Position, h);*/

			fPressure += -m * m *
				(p.Pressure / pow(p.Density, 2) + pN->Pressure / pow(pN->Density, 2)) *
				SpikyGradient(p.Position, pN->Position, h);

			fViscosity += u * m * (pN->Velocity - p.Velocity) / pN->Density *
				ViscosityLaplacian(p.Position, pN->Position, h);

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

		// Click and drag force at debug particle
		//if (p.Id == pDebugId)
		if (pD && distance2(pD->Position, p.Position) < pow(1.0f, 2))
		{
			mat4 mView = camera->getMatView();
			vec3 up = vec3(mView[0].y, mView[1].y, mView[2].y);
			vec3 right = cross(camera->getDirectionVec(), up);
			fExternal = (mouseDelta.x * right + -mouseDelta.y * up) * m * 5;
		}

		p.Force = fPressure + fViscosity + fExternal + fSurfaceTension;
		//p.Force = fPressure + fViscosity + fExternal;
		if (gravityEnabled) p.Force += fGravity;
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
		//p.Color -= vec4(0.02f) * p.Density / restDensity;
		//p.Color += vec4(0.1f) * length(p.Velocity) / 2;

		// Color debug particles
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
}

// TODO Proper Collision response
void WCSPH::resolveCollision(Particle* p)
{
	resolveCollision(p->Position, p->Velocity);
}

void WCSPH::resolveCollision(glm::vec3& position, glm::vec3& velocity)
{
	float boxhalfwidth = 5.0f;

	if (position.y < -boxhalfwidth)
	{
		position.y = -boxhalfwidth;
		velocity.y = -velocity.y / 2;
	}
	else if (position.y > boxhalfwidth)
	{
		position.y = boxhalfwidth;
		velocity.y = -velocity.y / 2;
	}

	if (position.x < -boxhalfwidth)
	{
		position.x = -boxhalfwidth;
		velocity.x = -velocity.x / 2;
	}
	else if (position.x > boxhalfwidth)
	{
		position.x = boxhalfwidth;
		velocity.x = -velocity.x / 2;
	}

	if (position.z < -boxhalfwidth)
	{
		position.z = -boxhalfwidth;
		velocity.z = -velocity.z / 2;
	}
	else if (position.z > boxhalfwidth)
	{
		position.z = boxhalfwidth;
		velocity.z = -velocity.z / 2;
	}
}

// === Smoothing Kernels ===

float WCSPH::Poly6(vec3 pos1, vec3 pos2, float h)
{
	float coef = 315.0f / (64.0f * PI * pow(h, 9));
	float r2 = distance2(pos1, pos2);
	float h2 = h * h;
	if (r2 > h2) return 0.0f;
	return coef * pow(h2 - r2, 3);
}

vec3 WCSPH::Poly6Gradient(vec3 pos1, vec3 pos2, float h)
{
	float coef = -945.0f / (32.0f * PI * pow(h, 9));
	float r2 = distance2(pos1, pos2);
	float h2 = h * h;
	if (r2 > h2) return vec3(0);
	vec3 r = pos1 - pos2;
	return coef * r * pow(h2 - r2, 2);
}

vec3 WCSPH::SpikyGradient(vec3 pos1, vec3 pos2, float h)
{
	float coef = -45.0f / (PI * pow(h, 6));
	vec3 r = pos1 - pos2;
	float rDist = length(r);
	if (rDist > h || rDist < 0.00001f) return vec3(0);
	return coef * r / rDist * pow(h - rDist, 2);
}

float WCSPH::ViscosityLaplacian(vec3 pos1, vec3 pos2, float h)
{
	float coef = 45.0f / (PI * pow(h, 6));
	float rDist = glm::distance(pos1, pos2);
	if (rDist > h) return 0.0f;
	return coef * (h - rDist);
}

float WCSPH::SurfaceTensionC(glm::vec3 pos1, glm::vec3 pos2, float h)
{
	float coef = 32.0f / (PI * pow(h, 9));
	vec3 r = pos1 - pos2;
	float rDist = length(r);
	if (2 * rDist > h != rDist <= h)
	{
		return coef * pow(h - rDist, 3) * pow(rDist, 3);
	}
	else if (rDist > 0.0f != 2 * rDist <= h)
	{
		return coef * 2 * pow(h - rDist, 3) * pow(rDist, 3) - pow(h, 6) / 64;
	}
	else
	{
		return 0.0f;
	}
}

void WCSPH::clear()
{
	pDebugId = -1;
}

void WCSPH::setDebugParticle(int id)
{
	pDebugId = id;
}

float WCSPH::getRadius() const
{
	return radius;
}

float WCSPH::getSmoothingLength() const
{
	return smoothingLength;
}

// Get the particle that intersects with ray
int WCSPH::getParticleAtRay(vec3 ray) const
{
	// Iterate all cells within range, +-5 units box size
	// Note, use 5.5, cause of rounding error when getting hashkey
	int cells = (int)(5.5 / cellSize);
	for (int i = -cells; i <= cells; i++)
	{
		for (int j = -cells; j <= cells; j++)
		{
			for (int k = -cells; k <= cells; k++)
			{
				ivec3 cellIndex = ivec3(i, j, k);
				vec3 cellPos = vec3(cellIndex) * cellSize;

				// Test if cell is intersecting with ray
				if (isIntersectingRaySphere(ray, cellPos, cellSize))
				{
					//cout << cellIndex.x << " " << cellIndex.y << " " << cellIndex.z << endl;
					// Get particles in cell and test intersection with ray
					int key = getHashKey(cellIndex);
					auto itb = pGrid.equal_range(key);
					for (auto it = itb.first; it != itb.second; it++)
					{
						Particle* p = it->second;
						//p->Color = vec4(1, 0, 0, 1);

						if (isIntersectingRaySphere(ray, p->Position, radius))
						{
							return p->Id;
						}
					}
				}
			}
		}
	}

	return -1;
}

bool WCSPH::isIntersectingRaySphere(vec3 ray, vec3 spherePos, float radius) const
{
	vec3 vP = spherePos - camera->getPosition();
	if (dot(vP, ray) < 0.0f) return false; // Skip particles behind camera
	float hypo = dot(vP, ray); // Project to find hypotenuse
	float t2 = dot(vP, vP) - hypo * hypo; // Find tangent^2
	if (t2 > 0 && t2 < pow(radius, 2)) return true;
	return false;
}

void WCSPH::toggleGravity()
{
	gravityEnabled = !gravityEnabled;
}