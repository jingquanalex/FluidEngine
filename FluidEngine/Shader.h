#pragma once

#include "Includes.h"
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

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

class Shader
{

private:

	static std::vector<std::pair<GLuint, std::string>> programList;

	GLuint program;

	std::string readFile(std::string filename);
	GLuint makeProgram(std::string name);

public:

	static GLuint uboMatrices;
	static GLuint uboLighting;
	static void setupUniformBuffers();

	Shader(std::string shadername);
	~Shader();

	void update(float dt);

	GLuint getProgram() const;

};