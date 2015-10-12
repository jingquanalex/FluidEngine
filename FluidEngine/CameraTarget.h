#pragma once

#include "globalIncludes.h"
#include "Camera.h"

class CameraTarget : public Camera
{

private:

	glm::vec3 target;
	float yaw, pitch, distance;

	bool stateLookAround = false;

public:

	CameraTarget();
	~CameraTarget();

	void update(float dt);

	void mouse(int button, int state);
	void mouseMotion(int x, int y);

	void setTarget(glm::vec3 target);
	glm::vec3 getTarget() const;
};