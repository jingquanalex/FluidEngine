#include "Camera.h"

using namespace glm;
using namespace std;

extern int window_width;
extern int window_height;

// Input: width and height of the window as int.
Camera::Camera()
{
	position = vec3(2.0f, 3.0f, 5.0f);
	direction = vec3(0.0f, 0.0f, -1.0f);
	up = vec3(0.0f, 1.0f, 0.0f);
	fov = 85.0f;
	zNear = 0.001f;
	zFar = 1000.0f;

	lookSensitivity = 0.2f;
	smoothness = 50.0f;

	setResolution(window_width, window_height);
	updateViewMatrix();
}

Camera::~Camera()
{

}

// Input: The frame delta time.
// Call in main update loop.
void Camera::update(float dt)
{
	updateViewMatrix();
}

// Create projection matrix, maintain viewport aspect ratio.
void Camera::updateProjectionMatrix()
{
	matProjection = perspective(radians(fov), aspectRatio, zNear, zFar);
}

// Create view matrix based on position.
void Camera::updateViewMatrix()
{
	matView = lookAt(position, position + direction, up);
}

void Camera::mouse(int button, int state)
{

}

// Track the delta for the mouse movements
void Camera::mouseMotion(int x, int y)
{
	mouseDeltaX = x - mouseLastX;
	mouseDeltaY = y - mouseLastY;
	mouseLastX = x;
	mouseLastY = y;
}

void Camera::mouseMotionPassive(int x, int y)
{
	mouseDeltaX = x - mouseLastX;
	mouseDeltaY = y - mouseLastY;
	mouseLastX = x;
	mouseLastY = y;
}

void Camera::keyboard(int key)
{

}

void Camera::keyboardUp(int key)
{

}

void Camera::keyboardSpecial(int key)
{

}

void Camera::keyboardSpecialUp(int key)
{

}

// Accessors definitions

// Input: width and height of the window as int.
// Calculate aspect ratio of window, call on window reshape.
void Camera::setResolution(int width, int height)
{
	if (height == 0) height = 1;
	resolution = vec2((float)width, (float)height);
	aspectRatio = resolution.x / resolution.y;
	updateProjectionMatrix();
}

void Camera::setMaxSpeed(float maxspeed)
{
	this->maxSpeed = maxspeed;
}

void Camera::setAcceleration(float acceleration)
{
	this->acceleration = acceleration;
}

void Camera::setSmoothness(float smoothness)
{
	this->smoothness = smoothness;
}

void Camera::setLookSensitivity(float sensitivity)
{
	this->lookSensitivity = sensitivity;
}

mat4 Camera::getMatViewProjection() const
{
	return matProjection * matView;
}

mat4 Camera::getMatView() const
{
	return matView;
}

mat4 Camera::getMatProjection() const
{
	return matProjection;
}

vec2 Camera::getResolution() const
{
	return resolution;
}

float Camera::getMaxSpeed() const
{
	return maxSpeed;
}

float Camera::getAcceleration() const
{
	return acceleration;
}

float Camera::getSmoothness() const
{
	return smoothness;
}

float Camera::getLookSensitivity() const
{
	return lookSensitivity;
}