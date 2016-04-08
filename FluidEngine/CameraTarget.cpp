#include "CameraTarget.h"

using namespace glm;
using namespace std;

extern int window_width;
extern int window_height;

CameraTarget::CameraTarget(Object* target, float distance) : Camera()
{
	view = Views::Chase;
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
	if (!isActive) return;

	direction = normalize(targetPosition - position);

	updateViewMatrix();
}

void CameraTarget::updateViewMatrix()
{
	if (targetObject == nullptr)
	{
		vec3 dirVec = rotate(vec3(0, 0, 1), radians(yaw), vec3(0, 1, 0));
		vec3 right = cross(-dirVec, vec3(0, 1, 0));

		position = targetPosition + rotate(dirVec, radians(pitch), right) * distance;
		matView = lookAt(position, targetPosition, vec3(0, 1, 0));
	}
	else
	{
		// Airplane Camera stuff

		mat4 matRotation = targetObject->getRotationMatrix();
		vec3 forward = vec3(matRotation[2][0], matRotation[2][1], matRotation[2][2]);
		vec3 up = vec3(matRotation[1][0], matRotation[1][1], matRotation[1][2]);
		vec3 right = vec3(matRotation[0][0], matRotation[0][1], matRotation[0][2]);

		switch (view)
		{
		case Views::Chase:
			if (stateLookAround)
			{
				vec3 dirVec = rotate(vec3(0, 0, 1), radians(yaw), vec3(0, 1, 0));
				vec3 right = cross(-dirVec, vec3(0, 1, 0));

				position = targetPosition + rotate(dirVec, radians(pitch), right) * distance;
				matView = lookAt(position, targetPosition, vec3(0, 1, 0));
			}
			else
			{
				position = targetPosition + rotate(-forward, radians(-defaultPitch), right) * distance;
				matView = lookAt(position, targetPosition, up);
			}
			break;

		case Views::Cockpit:
			position = targetPosition + forward;
			matView = lookAt(position, position + forward, up);
			break;

		case Views::Left:
			position = targetPosition + right * 1.5 - up * 0.5;
			matView = lookAt(position, position + forward, up);
			break;

		case Views::Right:
			position = targetPosition - forward * 3.0 - right * 1.5 - up * 0.5;
			matView = lookAt(position, position + forward, up);
			break;

		case Views::Ground:
			matView = lookAt(groundObject->getPosition(), targetPosition, vec3(0, 1, 0));
			break;

		case Views::Sky:
			matView = lookAt(groundObject->getPosition(), targetPosition, vec3(0, 1, 0));
			break;
		}
	}

	if (!isActive) return;

	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(getMatView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboLighting);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(position));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CameraTarget::mouse(int button, int state)
{
	if (!isActive) return;

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		stateLookAround = true;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		stateLookAround = false;
	}

	// Airplane stuff
	/*if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		if (view == Views::Chase)
		{
			stateLookAround = true;
			// Maintain cursor center position while moving mouse
			mouseTriggered = true;
			isWrappingPointer = true;
			glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
			mouseLastX = glutGet(GLUT_WINDOW_WIDTH) / 2;
			mouseLastY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
			yaw = defaultYaw;
			pitch = defaultPitch;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		if (view == Views::Chase)
		{
			stateLookAround = false;
			//mouseTriggered = false;
		}
	}*/
}

// Track the delta for the mouse movements
void CameraTarget::mouseMotion(int x, int y)
{
	if (!isActive) return;

	Camera::mouseMotion(x, y);

	if (stateLookAround)
	{
		// Update yaw and pitch and limit pitch
		yaw += -mouseDeltaX * mouseSensitivity;
		pitch += -mouseDeltaY * mouseSensitivity;
		pitch = clamp(pitch, -89.0f, 89.0f);
	}
}

void CameraTarget::mouseMotionPassive(int x, int y)
{
	if (!isActive) return;

	Camera::mouseMotionPassive(x, y);
}

void CameraTarget::mouseWheel(int dir)
{
	if (!isActive) return;

	if (stateLookAround)
	{
		distance += -dir * 0.5f;
		distance = clamp(distance, 0.0001f, 100.0f);
	}
}

void CameraTarget::keyboard(unsigned char key)
{
	if (!isActive) return;

	switch (key)
	{
	case '1':
		view = Views::Chase;
		break;
	case '2':
		view = Views::Cockpit;
		break;
	case '3':
		view = Views::Left;
		break;
	case '4':
		view = Views::Right;
		break;
	case '5':
		view = Views::Ground;
		break;
	case '6':
		view = Views::Sky;
		break;
	}
}

void CameraTarget::keyboardUp(unsigned char key)
{

}

void CameraTarget::setTargetObject(Object* target, Object* target2)
{
	this->targetObject = target;
	this->groundObject = target2;
	setTarget(target->getPosition());
}

void CameraTarget::setTarget(glm::vec3 target)
{
	this->targetPosition = target;
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