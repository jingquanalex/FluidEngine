#include "Light.h"

using namespace glm;
using namespace std;



Light::Light()
{
	position = vec3(10, 10, 10);
	ambientColor = vec3(0.1, 0.1, 0.2);
	diffuseColor = vec3(0.7, 0.8, 0.9);
	specularColor = vec3(1.0, 1.0, 1.0);

	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboLighting);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(vec4), sizeof(vec4), value_ptr(position));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(vec4), sizeof(vec4), value_ptr(ambientColor));
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(vec4), sizeof(vec4), value_ptr(diffuseColor));
	glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(vec4), sizeof(vec4), value_ptr(specularColor));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Light::~Light()
{

}

glm::vec3 Light::getPosition() const
{
	return position;
}

glm::vec3 Light::getAmbientColor() const
{
	return ambientColor;
}

glm::vec3 Light::getDiffuseColor() const
{
	return diffuseColor;
}

glm::vec3 Light::getSpecularColor() const
{
	return specularColor;
}

void Light::setPosition(glm::vec3 position)
{
	this->position = position;
	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboLighting);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(vec4), sizeof(vec4), value_ptr(position));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Light::setAmbientColor(glm::vec3 color)
{
	this->ambientColor = color;
	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboLighting);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(vec4), sizeof(vec4), value_ptr(ambientColor));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Light::setDiffuseColor(glm::vec3 color)
{
	this->diffuseColor = color;
	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboLighting);
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(vec4), sizeof(vec4), value_ptr(diffuseColor));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Light::setSpecularColor(glm::vec3 color)
{
	this->specularColor = color;
	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboLighting);
	glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(vec4), sizeof(vec4), value_ptr(specularColor));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}