#include "Includes.h"

struct Particle
{
	glm::vec3 Position = glm::vec3(0);
	glm::vec3 Velocity = glm::vec3(0);
	float Density = 0.0f;
	float Pressure = 0.0f;
	glm::vec4 Color = glm::vec4(1);
	std::vector<Particle*> Neighbors;

	/*bool operator==(const Particle &p)
	{
		return Position.x == p.Position.x && Position.y == p.Position.y && Position.z == p.Position.z;
	}*/
};