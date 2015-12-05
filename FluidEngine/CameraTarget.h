#pragma once

#include "Includes.h"
#include "Camera.h"
#include "Object.h"

class CameraTarget : public Camera
{

private:

	enum class Views { Cockpit, Chase, Left, Right, Ground, Sky };

	Object* targetObject;
	Object* groundObject;
	float defaultYaw, defaultPitch;
	float yaw, pitch, distance;

	Views view;
	bool stateLookAround = false;

	void updateViewMatrix();

public:

	CameraTarget(Object* target = nullptr, float distance = 5.0f);
	~CameraTarget();

	void update(float dt);

	void mouse(int button, int state);
	void mouseMotion(int x, int y);
	void mouseMotionPassive(int x, int y);
	void mouseWheel(int dir);
	void keyboard(unsigned char key);
	void keyboardUp(unsigned char key);

	void setTargetObject(Object* target, Object* target2 = nullptr);
	void setDistance(float distance);
	void setOrientation(float yaw, float pitch);
	Object* getTargetObject() const;
	float getDistance() const;
};