#pragma once

#include "Includes.h"
#include "Shader.h"

class Light
{

private:

	glm::vec3 position;
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

public:

	Light();
	~Light();

	glm::vec3 getPosition() const;
	glm::vec3 getAmbientColor() const;
	glm::vec3 getDiffuseColor() const;
	glm::vec3 getSpecularColor() const;

	void setPosition(glm::vec3 position);
	void setAmbientColor(glm::vec3 color);
	void setDiffuseColor(glm::vec3 color);
	void setSpecularColor(glm::vec3 color);

};