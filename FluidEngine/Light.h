#pragma once

#include "Includes.h"
#include "Shader.h"
#include "Camera.h"

class Light
{

private:

	glm::vec3 position;
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

	Shader* shader;
	GLuint depthRbo;
	GLuint depthFbo;
	GLuint depthMap;
	glm::ivec2 depthMapSize;
	glm::mat4 matLight;

public:

	Light();
	~Light();

	void load();
	void update(Camera* camera);

	glm::vec3 getPosition() const;
	glm::vec3 getAmbientColor() const;
	glm::vec3 getDiffuseColor() const;
	glm::vec3 getSpecularColor() const;

	void setPosition(glm::vec3 position);
	void setAmbientColor(glm::vec3 color);
	void setDiffuseColor(glm::vec3 color);
	void setSpecularColor(glm::vec3 color);

	GLuint getDepthFbo() const;
	GLuint getDepthMap() const;
	glm::ivec2 getDepthMapSize() const;
	GLuint getProgram() const;
	glm::mat4 getMatLight() const;

};