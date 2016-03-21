#pragma once

#include "WCSPH.h"
#include <iostream>
#include <fstream>

class PCISPH : public WCSPH
{

private:

	glm::vec3 fViscosity;
	glm::vec3 fGravity;
	glm::vec3 fExternal;
	glm::vec3 fPressure;

	float densityVariationThreshold;
	float scalingFactorDelta;

	float elasped = 0.0f;
	FILE * dFile;

public:

	PCISPH(float dt, std::vector<Particle>* particles, Camera* camera);

	void initialize();
	void compute(glm::ivec2 mouseDelta);
};