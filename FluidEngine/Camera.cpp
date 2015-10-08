#include "Camera.h"

using namespace glm;
using namespace std;

// Input: width and height of the window as int.
Camera::Camera(int width, int height)
{
	position = vec3(0.0f, 0.0f, 5.0f);
	direction = vec3(0.0f, 0.0f, -1.0f);
	up = vec3(0.0f, 1.0f, 0.0f);
	target = vec3(0.0f);
	fov = 85.0f;
	nearPlane = 0.001f;
	farPlane = 1000.0f;

	setResolution(width, height);
	updateViewMatrix();
}

Camera::~Camera()
{

}

// Input: width and height of the window as int.
// Calculate aspect ratio of window, call on window reshape.
void Camera::setResolution(int width, int height)
{
	if (height == 0) height = 1;
	resolution = vec2((float)width, (float)height);
	aspectRatio = resolution.x / resolution.y;
	updateProjectionMatrix();
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
	matProjection = perspective(radians(fov), aspectRatio, nearPlane, farPlane);
}

// Create view matrix based on position.
void Camera::updateViewMatrix()
{
	matView = lookAt(position, position + direction, up);
}

// Accessors definitions

void Camera::setPosition(glm::vec3 position)
{
	this->position = vec3(position.x, position.y, position.z);
}

void Camera::setTarget(glm::vec3 target)
{
	this->target = vec3(target.x, target.y, target.z);
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

vec3 Camera::getPosition() const
{
	return position;
}

vec3 Camera::getTarget() const
{
	return target;
}

vec2 Camera::getResolution() const
{
	return resolution;
}