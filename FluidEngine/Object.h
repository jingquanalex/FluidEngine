#pragma once

#include "Includes.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"

struct Material
{
	Material() : ambientColor(glm::vec3(0.4, 0.3, 0.4)), diffuseColor(glm::vec3(0.9, 0.9, 0.3)),
		specularColor(glm::vec3(0.3, 0.5, 0.8)), shininess(64.0f) { }

	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float shininess;
};

class Object
{

private:

	static GLuint defaultTexId;

protected:

	glm::mat4 matModel, matNormal;
	glm::vec3 position, rotation, scale;
	Material material;

	Model* model;
	Shader* shader;

	void updateModelMatrix();
	void updateNormalMatrix();

public:

	Object(glm::vec3 position = glm::vec3());
	~Object();

	virtual void load(std::string modelname, std::string shadername = "basic");
	virtual void update(float dt);
	virtual void draw();

	// === Accessors ===

	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);

	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;
	glm::vec3 getScale() const;
};