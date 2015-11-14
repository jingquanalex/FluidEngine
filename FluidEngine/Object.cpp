#include "Object.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

GLuint Object::defaultTexId;

Object::Object(vec3 position)
{
	model = nullptr;
	shader = nullptr;

	this->position = position;
	this->rotation = vec3(0.0);
	this->scale = vec3(1.0);

	updateModelMatrix();
	matNormal = mat4();
}

Object::~Object()
{

}

void Object::load(string modelname, string shadername)
{
	model = new Model(modelname);
	shader = new Shader(shadername);

	// Create default white diffuse texture
	if (defaultTexId == NULL)
	{
		defaultTexId = SOIL_load_OGL_texture((g_mediaDirectory + "white.png").c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

		glBindTexture(GL_TEXTURE_2D, defaultTexId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);

		glUniform1i(glGetUniformLocation(shader->getProgram(), "diffuse1"), defaultTexId);
	}
}

void Object::update(float dt)
{
	if (shader != nullptr) shader->update(dt);
}

void Object::draw()
{
	if (shader != nullptr)
	{
		GLuint program = shader->getProgram();
		glUseProgram(program);
		glUniformMatrix4fv(10, 1, GL_FALSE, value_ptr(matModel));
		glUniformMatrix4fv(11, 1, GL_FALSE, value_ptr(matNormal));
		glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, value_ptr(material.ambientColor));
		glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, value_ptr(material.diffuseColor));
		glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, value_ptr(material.specularColor));
		glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, defaultTexId);
		if (model != nullptr) model->draw(program);
	}
}

void Object::updateModelMatrix()
{
	matModel = glm::scale(scale) * eulerAngleXYZ(rotation.x, rotation.y, rotation.z) * translate(position);
}

void Object::updateNormalMatrix()
{
	matNormal = transpose(inverse(matModel));
}

void Object::setPosition(vec3 position)
{
	this->position = position;
	updateModelMatrix();
}

void Object::setRotation(vec3 rotation)
{
	this->rotation = vec3(radians(rotation.x), radians(rotation.y), radians(rotation.z));
	updateModelMatrix();
	updateNormalMatrix();
}

void Object::setScale(glm::vec3 scale)
{
	this->scale = scale;
	updateModelMatrix();
	updateNormalMatrix();
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