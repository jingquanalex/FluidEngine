#include "Light.h"

using namespace glm;
using namespace std;

Light::Light()
{
	position = vec3(10, 10, 10);
	ambientColor = vec3(0.2, 0.2, 0.3);
	diffuseColor = vec3(0.7, 0.8, 0.9);
	specularColor = vec3(0.5, 0.5, 0.5);

	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboLighting);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(vec4), sizeof(vec4), value_ptr(position));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(vec4), sizeof(vec4), value_ptr(ambientColor));
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(vec4), sizeof(vec4), value_ptr(diffuseColor));
	glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(vec4), sizeof(vec4), value_ptr(specularColor));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	depthMapSize = ivec2(4096, 4096);
}

Light::~Light()
{

}

void Light::load()
{
	shader = new Shader("depth");

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, depthMapSize.x, depthMapSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthMapSize.x, depthMapSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	/*glGenRenderbuffers(1, &depthRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, depthMapSize.x, depthMapSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);*/

	glGenFramebuffers(1, &depthFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthMap, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_RENDERBUFFER, depthRbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::update(Camera* camera)
{
	this->matLight = camera->getMatViewProjection();
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

GLuint Light::getDepthFbo() const
{
	return depthFbo;
}

GLuint Light::getDepthMap() const
{
	return depthMap;
}

ivec2 Light::getDepthMapSize() const
{
	return depthMapSize;
}

GLuint Light::getProgram() const
{
	return shader->getProgram();
}

glm::mat4 Light::getMatLight() const
{
	return matLight;
}