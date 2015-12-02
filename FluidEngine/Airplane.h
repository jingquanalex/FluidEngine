#pragma once

#include "Object.h"

class Airplane : public Object
{

private:

	bool isAirborne, isEnabled;
	float acceleration;
	float speed, maxAirborneSpeed, minAirborneSpeed;
	float yaw, pitch, roll;
	glm::vec3 defaultRotationAcceleration;
	glm::vec3 rotationAcceleration;
	glm::vec3 rotationSpeed;
	glm::vec3 rotationMaxSpeed;
	glm::vec3 forward;

	bool stateAccelerate = false;
	bool stateDecelerate = false;
	bool stateYawLeft = false;
	bool stateYawRight = false;
	bool stateRollLeft = false;
	bool stateRollRight = false;
	bool statePitchUp = false;
	bool statePitchDown = false;

	int mouseX, mouseY, mouseLastX, mouseLastY;
	float mouseDeltaX, mouseDeltaY;

	BoundingBox bb1, bb2;

	void updateModelMatrix();

public:

	Airplane(glm::vec3 position = glm::vec3());
	~Airplane();

	void load(std::string modelname = "plane.obj", std::string shadername = "basic");
	void update(float dt);

	void keyboard(int key);
	void keyboardUp(int key);
	void keyboardSpecial(int key);
	void keyboardSpecialUp(int key);

};