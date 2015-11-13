#pragma once

#include "Includes.h"
#include "Camera.h"

class CameraTarget : public Camera
{

private:

	glm::vec3 target;
	float yaw, pitch, distance;

	bool stateLookAround = false;

	void updateViewMatrix();

public:

	CameraTarget(glm::vec3 target, float distance);
	~CameraTarget();

	void update(float dt);

	void mouse(int button, int state);
	void mouseMotion(int x, int y);
	void mouseWheel(int dir);

	void setTarget(glm::vec3 target);
	glm::vec3 getTarget() const;
};