#pragma once

#include "Includes.h"
#include "Shader.h"

// Camera class that builds the projection and view matrices.
// Base class to build specialized cameras.
class Camera
{

protected:

	glm::mat4 matProjection, matView;
	glm::vec3 position, direction, up;
	float aspectRatio, fov, zNear, zFar;
	glm::vec2 resolution;

	glm::vec3 velocity;
	int mouseX, mouseY, mouseLastX, mouseLastY;
	float mouseDeltaX, mouseDeltaY;
	float maxSpeed, acceleration, smoothness;
	float mouseSensitivity;

	virtual void updateProjectionMatrix();
	virtual void updateViewMatrix();

public:

	Camera();
	~Camera();

	// === Functions ===

	virtual void update(float dt);
	virtual void mouse(int button, int state);
	virtual void mouseMotion(int x, int y);
	virtual void mouseMotionPassive(int x, int y);
	virtual void mouseWheel(int dir);
	virtual void keyboard(int key);
	virtual void keyboardUp(int key);
	virtual void keyboardSpecial(int key);
	virtual void keyboardSpecialUp(int key);

	// === Accessors ===

	void setPosition(glm::vec3 position);
	void setResolution(int width, int height);
	void setMaxSpeed(float maxspeed);
	void setAcceleration(float acceleration);
	void setSmoothness(float smoothness);
	void setMouseSensitivity(float sensitivity);

	glm::mat4 getMatViewProjection() const;
	glm::mat4 getMatView() const;
	glm::mat4 getMatProjection() const;
	glm::vec2 getResolution() const;
	glm::vec3 getPosition() const;
	float getMaxSpeed() const;
	float getAcceleration() const;
	float getSmoothness() const;
	float getMouseSensitivity() const;

};