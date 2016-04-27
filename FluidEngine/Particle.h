#pragma once

#include "Includes.h"

// Particle data to upload to GPU
struct sParticle
{
	glm::vec3 Position;
	glm::vec4 Color;
	float Radius;
};

class Particle
{

public:

	static int ID;

	int Id;
	glm::vec3 Position = glm::vec3(0);
	glm::vec3 Velocity = glm::vec3(0);
	glm::vec3 PositionPredicted = glm::vec3(0);
	glm::vec3 VelocityPredicted = glm::vec3(0);
	glm::vec3 NonPressureForce = glm::vec3(0);
	glm::vec3 Force = glm::vec3(0);
	glm::vec3 OldAcceleration = glm::vec3(0);
	glm::vec3 OldAccelerationPredicted = glm::vec3(0);
	float Density = 0.0f;
	float DensityPredicted = 0.0f;
	float DensityVariation = 0.0f;
	float Pressure = 0.0f;
	glm::vec3 PressureForce = glm::vec3(0);
	glm::vec3 Normal = glm::vec3(0);
	glm::vec4 Color = glm::vec4(1);
	std::vector<Particle*> Neighbors;

	float distance = 0.0f;

	Particle();

	bool operator==(const Particle &p);
};