#include "Object.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Object::Object(vec3 position)
{
	this->position = position;
	this->rotation = vec3(0.0);
	this->scale = vec3(1.0);
}

Object::~Object()
{

}

void Object::load(string modelname, string shadername)
{
	model = new Model(modelname);
	shader = new Shader(shadername);
}

void Object::load(string shadername)
{
	model = NULL;
	shader = new Shader(shadername);
}

void Object::update(float dt, Camera* camera)
{
	shader->update(dt, camera);
	matModel = glm::scale(scale) * eulerAngleXYZ(rotation.x, rotation.y, rotation.z) * translate(position);
}

void Object::draw()
{
	glUseProgram(shader->getProgram());
	glUniformMatrix4fv(10, 1, GL_FALSE, value_ptr(matModel));

	if (model != NULL) model->draw(shader->getProgram());

	glUseProgram(0);
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