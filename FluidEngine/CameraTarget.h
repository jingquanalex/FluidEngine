#pragma once

#include "Includes.h"
#include "Camera.h"
#include "Object.h"

class CameraTarget : public Camera
{

private:

	Object* targetObject;
	float yaw, pitch, distance;

	bool stateLookAround = false;

	void updateViewMatrix();

public:

	CameraTarget(Object* target = nullptr, float distance = 5.0f);
	~CameraTarget();

	void update(float dt);

	void mouse(int button, int state);
	void mouseMotion(int x, int y);
	void mouseWheel(int dir);

	void setTargetObject(Object* target);
	void setDistance(float distance);
	void setOrientation(float yaw, float pitch);
	Object* getTargetObject() const;
	float getDistance() const;
};