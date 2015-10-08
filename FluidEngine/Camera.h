#pragma once

#include "globalIncludes.h"

// Camera class that builds the projection and view matrices.
class Camera
{

protected:

	glm::mat4 matProjection, matView;
	glm::vec3 position, direction, up, target;
	float aspectRatio, fov, nearPlane, farPlane;
	glm::vec2 resolution;

	float moveSpeed;
	glm::vec3 velocity;

	void updateProjectionMatrix();
	void updateViewMatrix();

public:

	Camera(int width, int height);
	~Camera();

	// === Functions ===

	virtual void update(float dt);
	void setResolution(int width, int height);

	// === Accessors ===

	void setPosition(glm::vec3 position);
	void setTarget(glm::vec3 target);

	glm::mat4 getMatViewProjection() const;
	glm::mat4 getMatView() const;
	glm::mat4 getMatProjection() const;
	glm::vec3 getPosition() const;
	glm::vec3 getTarget() const;
	glm::vec2 getResolution() const;
	
};