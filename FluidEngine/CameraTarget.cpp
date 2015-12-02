#include "CameraTarget.h"

using namespace glm;
using namespace std;

extern int window_width;
extern int window_height;

CameraTarget::CameraTarget(Object* target, float distance) : Camera()
{
	defaultYaw = yaw = 0.0f;
	defaultPitch = pitch = 0.0f;
	this->targetObject = target;
	this->distance = distance;

	updateViewMatrix();
}

CameraTarget::~CameraTarget()
{

}

void CameraTarget::update(float dt)
{
	if (!isActive || targetObject == nullptr) return;

	mouseDeltaX = mouseSensitivity * (mouseX - mouseLastX);
	mouseDeltaY = mouseSensitivity * (mouseY - mouseLastY);
	mouseLastX = mouseX;
	mouseLastY = mouseY;

	updateViewMatrix();
}

void CameraTarget::updateViewMatrix()
{
	if (targetObject == nullptr) return;

	if (stateLookAround)
	{
		vec3 dirVec = rotate(vec3(0, 0, 1), radians(yaw), up);
		vec3 right = cross(-dirVec, up);

		position = targetObject->getPosition() + rotate(dirVec, radians(pitch), right) * distance;
		matView = lookAt(position, targetObject->getPosition(), up);
	}
	else
	{
		mat4 matRotation = targetObject->getRotationMatrix();
		vec3 forward = vec3(matRotation[2][0], matRotation[2][1], matRotation[2][2]);
		vec3 up = vec3(matRotation[1][0], matRotation[1][1], matRotation[1][2]);
		vec3 right = vec3(matRotation[0][0], matRotation[0][1], matRotation[0][2]);

		position = targetObject->getPosition() + rotate(-forward, radians(-defaultPitch), right) * distance;
		matView = lookAt(position, targetObject->getPosition(), up);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(getMatView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboLighting);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(position));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CameraTarget::mouse(int button, int state)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		stateLookAround = true;
		mouseTriggered = true;
		isWrappingPointer = true;
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		mouseLastX = glutGet(GLUT_WINDOW_WIDTH) / 2;
		mouseLastY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		stateLookAround = false;
		mouseTriggered = false;
		yaw = defaultYaw;
		pitch = defaultPitch;

	}
}

// Track the delta for the mouse movements
void CameraTarget::mouseMotion(int x, int y)
{
	if (stateLookAround)
	{
		Camera::mouseMotion(x, y);

		// Update yaw and pitch and limit pitch
		yaw += -mouseDeltaX * mouseSensitivity;
		pitch += -mouseDeltaY * mouseSensitivity;
		pitch = clamp(pitch, -89.0f, 89.0f);
	}
}

void CameraTarget::mouseMotionPassive(int x, int y)
{
	if (stateLookAround)
	{
		Camera::mouseMotionPassive(x, y);
	}
}

void CameraTarget::mouseWheel(int dir)
{
	//if (stateLookAround)
	{
		distance += -dir;
		distance = clamp(distance, 0.0f, 100.0f);
	}
}

void CameraTarget::setTargetObject(Object* target)
{
	this->targetObject = target;
}

void CameraTarget::setDistance(float distance)
{
	this->distance = distance;
}

void CameraTarget::setOrientation(float yaw, float pitch)
{
	defaultYaw = this->yaw = yaw;
	defaultPitch = this->pitch = pitch;
}

Object* CameraTarget::getTargetObject() const
{
	return targetObject;
}

float CameraTarget::getDistance() const
{
	return distance;
}