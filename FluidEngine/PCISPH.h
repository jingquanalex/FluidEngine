#pragma once

#include "WCSPH.h"

class PCISPH : public WCSPH
{

private:

	glm::vec3 fViscosity;
	glm::vec3 fGravity;
	glm::vec3 fExternal;
	glm::vec3 fPressure;

	float densityVariationThreshold;
	float scalingFactor;

public:

	PCISPH(std::vector<Particle>* particles, Camera* camera);

	void compute(float dt, glm::ivec2 mouseDelta);

};