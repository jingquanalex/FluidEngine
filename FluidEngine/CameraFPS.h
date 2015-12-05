#pragma once

#include "Includes.h"
#include "Camera.h"

class CameraFPS : public Camera
{

private:

	float yaw, pitch;
	float moveSpeed;

	bool stateLookAround = false;
	bool stateForward = false, stateBackward = false, 
		stateLeft = false, stateRight = false;

public:

	CameraFPS();
	~CameraFPS();

	void update(float dt);

	void mouse(int button, int state);
	void mouseMotion(int x, int y);
	void keyboard(int key);
	void keyboardUp(int key);

	void setTargetPoint(glm::vec3 position);

};