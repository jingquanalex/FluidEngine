#include "Airplane.h"

using namespace glm;
using namespace std;

Airplane::Airplane(vec3 position) : Object(position)
{
	hasCollided = false;
	hasCrashed = false;
	isAirborne = false;

	mouseLastX = 0;
	mouseLastY = 0;
	acceleration = 5.0f;
	speed = 0.0f;
	maxAirborneSpeed = 30.0f;
	minAirborneSpeed = 15.0f;
	defaultPosition = position;
	defaultRotationAcceleration = vec3(110, 110, 190);
	rotationAcceleration = defaultRotationAcceleration;
	rotationSpeed = vec3(0);
	rotationMaxSpeed = vec3(50, 70, 130);
	glm::vec3 rotationAcceleration;
	glm::vec3 rotationMaxSpeed;
	forward = vec3(0, 0, 1);
}

Airplane::~Airplane()
{

}

void Airplane::setupDefaults()
{
	hasCollided = false;
	hasCrashed = false;
	isAirborne = false;
	acceleration = 5.0f;
	speed = 0.0f;
	position = defaultPosition;
	rotationAcceleration = defaultRotationAcceleration;
	rotationSpeed = vec3(0);
	forward = vec3(0, 0, 1);
	matRotation = mat4();
}

void Airplane::load(string modelname, string shadername)
{
	Object::load(modelname, shadername);

	// Make 2 bounding boxes and line strips buffer
	bb1 = BoundingBox(
		model->getBoundingBox().Min + vec3(4.2, 0, 0),
		model->getBoundingBox().Max - vec3(4.2, 0, 0));
	bb2 = BoundingBox(
		model->getBoundingBox().Min + vec3(0, 0.55, 3.35),
		model->getBoundingBox().Max - vec3(0, 0.3, 2.5));

	listBoundingBox.clear();
	listBoundingBox.push_back(bb1);
	listBoundingBox.push_back(bb2);

	makeBoundingBoxData();

	// Reinitialize vbo memory
	glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
	glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(vec3), NULL, GL_STREAM_DRAW);
}

void Airplane::update(float dt)
{
	if (hasCrashed) return;

	mouseDeltaX = (float)(mouseX - mouseLastX);
	mouseDeltaY = (float)(mouseY - mouseLastY);
	mouseLastX = mouseX;
	mouseLastY = mouseY;

	yaw = pitch = roll = 0.0f;

	// Slower rotation acceleration if speed is slow
	if (isAirborne)
	{
		rotationAcceleration = defaultRotationAcceleration * glm::max(speed / maxAirborneSpeed, 0.4f);
	}
	else
	{
		rotationAcceleration = defaultRotationAcceleration * glm::max(speed / maxAirborneSpeed, 0.0f);
	}

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

	if (speed == 0.0f) rotationSpeed = vec3(0);

	yaw = rotationSpeed.y * dt;
	pitch = rotationSpeed.x * dt;
	roll = rotationSpeed.z * dt;

	quat qx = angleAxis(radians(pitch), vec3(matRotation[0][0], matRotation[0][1], matRotation[0][2]));
	quat qy = angleAxis(radians(yaw), vec3(matRotation[1][0], matRotation[1][1], matRotation[1][2]));
	quat qz = angleAxis(radians(roll), vec3(matRotation[2][0], matRotation[2][1], matRotation[2][2]));
	quat qrot = qz * qx * qy;

	// If plane is on ground, orient it horizontally if not
	if (!isAirborne && speed <= minAirborneSpeed)
	{
		vec3 newForward = cross(vec3(0.0f, 1.0f, 0.0f), vec3(matRotation[0][0], matRotation[0][1], matRotation[0][2]));
		quat qhorizontal = rotationBetweenVectors(forward, -newForward);
		qhorizontal *= rotationBetweenVectors(vec3(matRotation[1][0], matRotation[1][1], matRotation[1][2]), vec3(0.0f, 1.0f, 0.0f));
		qrot *= mix(qrot, qhorizontal, 2.14f * dt);
	}

	matRotation = mat4_cast(qrot) * matRotation;
	forward = vec3(mat4_cast(qrot) * vec4(forward.x, forward.y, forward.z, 1));

	/*matRotation = mat4(
		right.x, up.x, forward.x, 0.0f,
		right.y, up.y, forward.y, 0.0f,
		right.z, up.z, forward.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);*/

	if (stateAccelerate)
	{
		speed += acceleration * dt;
		speed = glm::min(speed, maxAirborneSpeed);
	}

	if (stateDecelerate)
	{
		speed -= acceleration * dt;

		// Maintain minimum speed when in flight
		if (isAirborne)
		{
			speed = glm::max(speed, minAirborneSpeed);
		}
		else
		{
			speed = glm::max(speed, 0.0f);
		}
	}

	// Decelerate automatically during landing (not in flight)
	if (stateAccelerate == stateDecelerate)
	{
		if (!isAirborne)
		{
			speed -= acceleration * dt;
			speed = glm::max(speed, 0.0f);
		}
	}

	lastPosition = position;
	position += forward * speed * dt;

	if (isAirborne)
	{
		// Make plane fall if too slow
		position.y -= (maxAirborneSpeed + 5 - speed) * 0.2f * dt;
	}

	if (hasCollided)
	{
		hasCrashed = true;
	}

	updateModelMatrix();
	updateNormalMatrix();

	Object::update(dt);
}

void Airplane::updateModelMatrix()
{
	matModel = translate(position) * matRotation;
}

quat Airplane::rotationBetweenVectors(vec3 start, vec3 dest)
{
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	vec3 rotationAxis = cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs);
}

void Airplane::keyboard(int key)
{
	switch (key)
	{
	case 'w':
		stateAccelerate = true;
		break;
	case 'a':
		stateYawLeft = true;
		break;
	case 's':
		stateDecelerate = true;
		break;
	case 'd':
		stateYawRight = true;
		break;
	case ' ':
		setupDefaults();
		break;
	}
}

void Airplane::keyboardUp(int key)
{
	switch (key)
	{
	case 'w':
		stateAccelerate = false;
		break;
	case 'a':
		stateYawLeft = false;
		break;
	case 's':
		stateDecelerate = false;
		break;
	case 'd':
		stateYawRight = false;
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

void Airplane::setHasCollided(bool hasCollided)
{
	this->hasCollided = hasCollided;
}

void Airplane::setIsAirborne(bool isAirborne)
{
	this->isAirborne = isAirborne;
}

bool Airplane::getIsAirborne() const
{
	return isAirborne;
}