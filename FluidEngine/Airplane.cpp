#include "Airplane.h"

using namespace glm;
using namespace std;

Airplane::Airplane(vec3 position) : Object(position)
{
	isWrappingPointer = false;
	isEnabled = true;
	isAirborne = false;
	mouseLastX = 0;
	mouseLastY = 0;
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	acceleration = 10.0f;
	speed = 0.0f;
	maxAirborneSpeed = 20.0f;
	minAirborneSpeed = 5.0f;
	rotationAcceleration = vec3(110, 110, 190);
	rotationSpeed = vec3(0);
	rotationMaxSpeed = vec3(50, 70, 130);
	glm::vec3 rotationAcceleration;
	glm::vec3 rotationMaxSpeed;
	forward = vec3(0, 0, 1);
}

Airplane::~Airplane()
{

}

void Airplane::load(string modelname, string shadername)
{
	Object::load(modelname, shadername);
}

void Airplane::update(float dt)
{
	mouseDeltaX = (float)(mouseX - mouseLastX);
	mouseDeltaY = (float)(mouseY - mouseLastY);
	mouseLastX = mouseX;
	mouseLastY = mouseY;

	yaw = pitch = roll = 0.0f;

	if (stateYawLeft)
	{
		rotationSpeed.y += rotationAcceleration.y * dt;
		rotationSpeed.y = rotationSpeed.y > rotationMaxSpeed.y ? rotationMaxSpeed.y : rotationSpeed.y;
	}
	if (stateYawRight)
	{
		rotationSpeed.y -= rotationAcceleration.y * dt;
		rotationSpeed.y = rotationSpeed.y < -rotationMaxSpeed.y ? -rotationMaxSpeed.y : rotationSpeed.y;
	}
	if (stateYawLeft == stateYawRight)
	{
		if (abs(rotationSpeed.y) > 0.5f)
		{
			rotationSpeed.y += clamp(-rotationSpeed.y, -1.0f, 1.0f) * rotationAcceleration.y * dt;
		}
		else
		{
			rotationSpeed.y = 0.0f;
		}
	}

	if (statePitchUp)
	{
		rotationSpeed.x += rotationAcceleration.x * dt;
		rotationSpeed.x = rotationSpeed.x > rotationMaxSpeed.x ? rotationMaxSpeed.x : rotationSpeed.x;
	}
	if (statePitchDown)
	{
		rotationSpeed.x -= rotationAcceleration.x * dt;
		rotationSpeed.x = rotationSpeed.x < -rotationMaxSpeed.x ? -rotationMaxSpeed.x : rotationSpeed.x;
	}
	if (statePitchUp == statePitchDown)
	{
		if (abs(rotationSpeed.x) > 0.5f)
		{
			rotationSpeed.x += clamp(-rotationSpeed.x, -1.0f, 1.0f) * rotationAcceleration.x * dt;
		}
		else
		{
			rotationSpeed.x = 0.0f;
		}
	}

	if (stateRollLeft)
	{
		rotationSpeed.z -= rotationAcceleration.z * dt;
		rotationSpeed.z = rotationSpeed.z < -rotationMaxSpeed.z ? -rotationMaxSpeed.z : rotationSpeed.z;
	}
	if (stateRollRight)
	{
		rotationSpeed.z += rotationAcceleration.z * dt;
		rotationSpeed.z = rotationSpeed.z > rotationMaxSpeed.z ? rotationMaxSpeed.z : rotationSpeed.z;
	}
	if (stateRollLeft == stateRollRight)
	{
		if (abs(rotationSpeed.z) > 0.5f)
		{
			rotationSpeed.z += clamp(-rotationSpeed.z, -1.0f, 1.0f) * rotationAcceleration.z * dt;
		}
		else
		{
			rotationSpeed.z = 0.0f;
		}
	}
	
	yaw = rotationSpeed.y * dt;
	pitch = rotationSpeed.x * dt;
	roll = rotationSpeed.z * dt;

	quat qy = angleAxis(radians(yaw), vec3(matRotation[1][0], matRotation[1][1], matRotation[1][2]));
	quat qx = angleAxis(radians(pitch), vec3(matRotation[0][0], matRotation[0][1], matRotation[0][2]));
	quat qz = angleAxis(radians(roll), vec3(matRotation[2][0], matRotation[2][1], matRotation[2][2]));
	matRotation = mat4_cast(qz * qx * qy) * matRotation;
	forward = vec3(mat4_cast(qz * qx * qy) * vec4(forward.x, forward.y, forward.z, 1));

	/*matRotation = mat4(
		right.x, up.x, forward.x, 0.0f,
		right.y, up.y, forward.y, 0.0f,
		right.z, up.z, forward.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);*/

	if (stateAccelerate)
	{
		speed += acceleration * dt;
		speed = speed > maxAirborneSpeed ? maxAirborneSpeed : speed;
	}
	if (stateDecelerate)
	{
		speed -= acceleration * dt;
		if (isAirborne)
		{
			speed = speed < minAirborneSpeed ? minAirborneSpeed : speed;
		}
		else
		{
			speed = speed < 0.0f ? 0.0f : speed;
		}
	}

	position += forward * speed * dt;

	updateModelMatrix();
	updateNormalMatrix();

	Object::update(dt);
}

void Airplane::updateModelMatrix()
{
	matModel = translate(position) * matRotation;
}

void Airplane::mouseMotion(int x, int y)
{
	if (!isEnabled) return;

	if (!isWrappingPointer)
	{
		mouseX = x;
		mouseY = y;

		if (mouseX != glutGet(GLUT_WINDOW_WIDTH) / 2 || mouseY != glutGet(GLUT_WINDOW_HEIGHT) / 2)
		{
			isWrappingPointer = true;
			glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		}
	}
	else
	{
		isWrappingPointer = false;
		mouseLastX = glutGet(GLUT_WINDOW_WIDTH) / 2;
		mouseLastY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	}
}

void Airplane::mouseMotionPassive(int x, int y)
{
	if (!isEnabled) return;

	// NOTE: glutWarpPointer calls mouseMotionPassive to move the mouse
	if (!isWrappingPointer)
	{
		mouseX = x;
		mouseY = y;

		if (mouseX != glutGet(GLUT_WINDOW_WIDTH) / 2 || mouseY != glutGet(GLUT_WINDOW_HEIGHT) / 2)
		{
			isWrappingPointer = true;
			glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		}
	}
	else
	{
		isWrappingPointer = false;
		mouseLastX = glutGet(GLUT_WINDOW_WIDTH) / 2;
		mouseLastY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	}
}

void Airplane::keyboard(int key)
{
	switch (key)
	{
	case 119: // w
		stateAccelerate = true;
		break;
	case 97: // a
		stateYawLeft = true;
		break;
	case 115: // s
		stateDecelerate = true;
		break;
	case 100: // d
		stateYawRight = true;
		break;
	}
}

void Airplane::keyboardUp(int key)
{
	switch (key)
	{
	case 119: // w
		stateAccelerate = false;
		break;
	case 97: // a
		stateYawLeft = false;
		break;
	case 115: // s
		stateDecelerate = false;
		break;
	case 100: // d
		stateYawRight = false;
		break;
	case 99: // c
		isEnabled = !isEnabled;
		if (isEnabled)
		{
			isWrappingPointer = true;
			glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
			mouseLastX = glutGet(GLUT_WINDOW_WIDTH) / 2;
			mouseLastY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
		}
		break;
	}
}

void Airplane::keyboardSpecial(int key)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		statePitchUp = true;
		break;
	case GLUT_KEY_DOWN:
		statePitchDown = true;
		break;
	case GLUT_KEY_LEFT:
		stateRollLeft = true;
		break;
	case GLUT_KEY_RIGHT:
		stateRollRight = true;
		break;
	}
}

void Airplane::keyboardSpecialUp(int key)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		statePitchUp = false;
		break;
	case GLUT_KEY_DOWN:
		statePitchDown = false;
		break;
	case GLUT_KEY_LEFT:
		stateRollLeft = false;
		break;
	case GLUT_KEY_RIGHT:
		stateRollRight = false;
		break;
	}
}