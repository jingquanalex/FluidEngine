#pragma once

#include "globalIncludes.h"
#include "Camera.h"

class CameraFPS : public Camera
{

private:

	float dt;
	int mouseDeltaX, mouseDeltaY, mouseLastX, mouseLastY;
	float sensitivity, smoothness, yaw, pitch;
	float moveSpeed;
	bool stateLookAround = false;
	bool stateForward = false, stateBackward = false, 
		stateLeft = false, stateRight = false;

public:

	CameraFPS(int width, int height);
	~CameraFPS();

	void update(float dt);

	void mouse(int button, int state);
	void mouseMotion(int x, int y);
	void mouseMotionPassive(int x, int y);
	void keyboard(int key);
	void keyboardUp(int key);

};