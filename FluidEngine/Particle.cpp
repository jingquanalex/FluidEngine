#include "Particle.h"

using namespace glm;
using namespace std;

int Particle::ID;

Particle::Particle()
{
	Id = ID++;
}

bool Particle::operator==(const Particle &p)
{
	return Id == p.Id;
}