#include "CameraFPS.h"

using namespace glm;
using namespace std;

extern int window_width;
extern int window_height;

CameraFPS::CameraFPS(int width, int height) : Camera(width, height)
{
	sensitivity = 0.2f;
	smoothness = 50.0f;
	yaw = -90;
	pitch = 0;
	moveSpeed = 10.0f;
}

CameraFPS::~CameraFPS()
{

}

void CameraFPS::update(float dt)
{
	Camera::update(dt);

	this->dt = dt;

	if (stateForward)
	{
		position += direction * moveSpeed * dt;
	}
	else if (stateBackward)
	{
		position -= direction * moveSpeed * dt;
	}

	if (stateLeft)
	{
		position -= normalize(cross(direction, up)) * moveSpeed * dt;
	}
	else if (stateRight)
	{
		position += normalize(cross(direction, up)) * moveSpeed * dt;
	}
}

void CameraFPS::mouse(int button, int state)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		stateLookAround = true;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		stateLookAround = false;
	}
}

// Track the delta for the mouse movements
void CameraFPS::mouseMotion(int x, int y)
{
	if (stateLookAround)
	{
		mouseDeltaX = x - mouseLastX;
		mouseDeltaY = y - mouseLastY;
		mouseLastX = x;
		mouseLastY = y;

		// Update yaw and pitch and limit pitch
		yaw += mouseDeltaX * sensitivity;
		pitch += -mouseDeltaY * sensitivity;

		//printf("%f, %f \n", yaw, pitch);

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		direction.x = cos(radians(pitch)) * cos(radians(yaw));
		direction.y = sin(radians(pitch));
		direction.z = cos(radians(pitch)) * sin(radians(yaw));
		direction = normalize(direction);
	}
}

void CameraFPS::mouseMotionPassive(int x, int y)
{
	mouseLastX = x;
	mouseLastY = y;
}

// Keys callback
void CameraFPS::keyboard(int key)
{
	switch (key)
	{
	case 119: // w
		stateForward = true;
		break;
	case 97: // a
		stateLeft = true;
		break;
	case 115: // s
		stateBackward = true;
		break;
	case 100: // d
		stateRight = true;
		break;
	}
}

// Keys up callback
void CameraFPS::keyboardUp(int key)
{
	switch (key)
	{
	case 119: // w
		stateForward = false;
		break;
	case 97: // a
		stateLeft = false;
		break;
	case 115: // s
		stateBackward = false;
		break;
	case 100: // d
		stateRight = false;
		break;
	}
}
