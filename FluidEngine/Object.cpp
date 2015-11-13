#include "Object.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Object::Object(vec3 position)
{
	model = nullptr;
	shader = nullptr;

	this->position = position;
	this->rotation = vec3(0.0);
	this->scale = vec3(1.0);
	this->color = vec3(0.3, 0.5, 0.1);
}

Object::~Object()
{

}

void Object::load(string modelname, string shadername)
{
	model = new Model(modelname);
	shader = new Shader(shadername);
}

void Object::update(float dt)
{
	matModel = glm::scale(scale) * eulerAngleXYZ(rotation.x, rotation.y, rotation.z) * translate(position);
	if (shader != nullptr) shader->update(dt);
}

void Object::draw()
{
	if (shader != nullptr)
	{
		glUseProgram(shader->getProgram());
		glUniformMatrix4fv(10, 1, GL_FALSE, value_ptr(matModel));
		glUniform3fv(20, 1, value_ptr(color));
		if (model != nullptr) model->draw(shader->getProgram());
		glUseProgram(0);
	}
}

void Object::setPosition(vec3 position)
{
	this->position = position;
}

void Object::setRotation(vec3 rotation)
{
	this->rotation = vec3(radians(rotation.x), radians(rotation.y), radians(rotation.z));
}

void Object::setScale(glm::vec3 scale)
{
	this->scale = scale;
}

vec3 Object::getPosition() const
{
	return position;
}

vec3 Object::getRotation() const
{
	return rotation;
}

vec3 Object::getScale() const
{
	return scale;
}