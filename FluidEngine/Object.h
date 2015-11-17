#pragma once

#include "Includes.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"

struct Material
{
private:
	glm::vec3 emissiveColor;
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float shininess;

public:
	Material() : 
		emissiveColor(glm::vec3(0.0, 0.0, 0.0)), 
		ambientColor(glm::vec3(0.4, 0.3, 0.4)), 
		diffuseColor(glm::vec3(0.9, 0.9, 0.3)), 
		specularColor(glm::vec3(0.3, 0.5, 0.8)), 
		shininess(64.0f) { }

	void setEmissiveColor(glm::vec3 color) { emissiveColor = color; }
	void setAmbientColor(glm::vec3 color) { ambientColor = color; }
	void setDiffuseColor(glm::vec3 color) { diffuseColor = color; }
	void setSpecularColor(glm::vec3 color) { specularColor = color; }
	void setShininess(float value) { shininess = value; }
	glm::vec3 getEmissiveColor() { return emissiveColor; }
	glm::vec3 getAmbientColor() { return ambientColor; }
	glm::vec3 getDiffuseColor() { return diffuseColor; }
	glm::vec3 getSpecularColor() { return specularColor; }
	float getShininess() { return shininess; }
};

class Object
{

protected:

	glm::mat4 matModel, matNormal;
	glm::vec3 position, rotation, scale;
	Material material;
	static GLuint defaultTexId;

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
	Material* getMaterial() { return &material; };
};