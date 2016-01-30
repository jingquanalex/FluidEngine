#include "Includes.h"

class Particle
{

private:

	static int ID;

public:

	int Id;
	glm::vec3 Position = glm::vec3(0);
	glm::vec3 Velocity = glm::vec3(0);
	glm::vec3 Force = glm::vec3(0);
	float Density = 0.0f;
	float Pressure = 0.0f;
	glm::vec4 Color = glm::vec4(1);
	std::vector<Particle*> Neighbors;

	Particle();

	bool operator==(const Particle &p);
};