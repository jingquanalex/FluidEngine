#include "Object.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Object::Object(vec3 position)
{
	this->position = position;
}

Object::~Object()
{

}

void Object::load(string modelname, string shadername)
{
	model = new Model(modelname);
	shader = new Shader(shadername);
}

void Object::update(float dt, Camera* camera)
{
	shader->update(dt, camera);

	// Apply object transformations
	matModel = translate(position);
}

void Object::draw()
{
	glUseProgram(shader->getProgram());
	glUniformMatrix4fv(10, 1, GL_FALSE, value_ptr(matModel));

	model->draw(shader->getProgram());

	glUseProgram(0);
}

void Object::setPosition(vec3 position)
{
	this->position = position;
}

vec3 Object::getPosition() const
{
	return position;
}