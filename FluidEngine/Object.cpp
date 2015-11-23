#include "Object.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

GLuint Object::defaultTexID;

Object::Object(vec3 position)
{
	model = nullptr;
	shader = nullptr;

	this->position = position;
	this->rotation = vec3(0.0);
	this->scale = vec3(1.0);

	updateModelMatrix();
	matNormal = mat4();
	matRotation = mat4();

	isDrawBoundingBox = false;
}

Object::~Object()
{

}

void Object::load(string modelname, string shadername)
{
	model = new Model(modelname);
	shader = new Shader(shadername);

	// Create default white diffuse texture
	if (defaultTexID == NULL)
	{
		defaultTexID = SOIL_load_OGL_texture((g_mediaDirectory + "grass.png").c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

		glBindTexture(GL_TEXTURE_2D, defaultTexID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Generate 2 bounding boxes and line strips buffer
	bb1 = BoundingBox(
		model->getBoundingBox().Min + vec3(4.2, 0, 0), 
		model->getBoundingBox().Max - vec3(4.2, 0, 0));
	bb2 = BoundingBox(
		model->getBoundingBox().Min + vec3(0, 0.55, 3.35),
		model->getBoundingBox().Max - vec3(0, 0.3, 2.5));

	makeBoundingBoxVertices(lineVertices);
	glGenVertexArrays(1, &lineVao);
	glGenBuffers(1, &lineVbo);

	glBindVertexArray(lineVao);
	glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
	glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(vec3), NULL, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glBindVertexArray(0);
}

void Object::update(float dt)
{
	if (shader != nullptr) shader->update(dt);

	if (isDrawBoundingBox)
	{
		makeBoundingBoxVertices(lineVertices);
		glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, lineVertices.size() * sizeof(vec3), &lineVertices[0]);
	}
}

void Object::draw()
{
	if (shader != nullptr)
	{
		GLuint program = shader->getProgram();
		glUseProgram(program);
		glUniformMatrix4fv(10, 1, GL_FALSE, value_ptr(matModel));
		glUniformMatrix4fv(11, 1, GL_FALSE, value_ptr(matNormal));
		glUniform3fv(glGetUniformLocation(program, "material.emissive"), 1, value_ptr(material.getEmissiveColor()));
		glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, value_ptr(material.getAmbientColor()));
		glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, value_ptr(material.getDiffuseColor()));
		glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, value_ptr(material.getSpecularColor()));
		glUniform1f(glGetUniformLocation(program, "material.shininess"), material.getShininess());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, defaultTexID);
		if (model != nullptr) model->draw(program);

		if (isDrawBoundingBox)
		{
			glUniformMatrix4fv(10, 1, GL_FALSE, value_ptr(mat4()));
			glBindVertexArray(lineVao);
			glDrawArrays(GL_LINE_STRIP, 0, lineVertices.size());
			glBindVertexArray(0);
		}
	}
}

void Object::updateModelMatrix()
{
	matRotation = eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	matModel = translate(position) * glm::scale(scale) * matRotation;
}

void Object::updateNormalMatrix()
{
	matNormal = transpose(inverse(matModel));
}

// Generate vertices for the two bounding boxes as a line strip transformed by model matrix
void Object::makeBoundingBoxVertices(std::vector<glm::vec3>& list)
{
	list.clear();

	BoundingBox bb = bb1;
	vec3 v1 = vec3(matModel * vec4(bb.Min.x, bb.Min.y, bb.Max.z, 1));
	list.push_back(v1);
	vec3 v2 = vec3(matModel * vec4(bb.Min.x, bb.Max.y, bb.Max.z, 1));
	list.push_back(v2);
	vec3 v3 = vec3(matModel * vec4(bb.Min.x, bb.Max.y, bb.Min.z, 1));
	list.push_back(v3);
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Min.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Min.y, bb.Max.z, 1)));
	vec3 v6 = vec3(matModel * vec4(bb.Max.x, bb.Min.y, bb.Max.z, 1));
	list.push_back(v6);
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Max.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Max.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Max.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Max.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Min.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Min.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Max.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Max.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Min.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Min.y, bb.Min.z, 1)));

	// Store face normals for seperating axis test
	faceNormals.push_back(v1 - v6); // X
	faceNormals.push_back(v2 - v1); // Y
	faceNormals.push_back(v2 - v3); // Z

	bb = bb2;
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Min.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Max.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Max.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Min.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Min.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Min.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Max.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Max.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Max.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Max.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Min.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Min.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Max.y, bb.Max.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Max.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Max.x, bb.Min.y, bb.Min.z, 1)));
	list.push_back(vec3(matModel * vec4(bb.Min.x, bb.Min.y, bb.Min.z, 1)));
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

void Object::setDrawBoundingBox(bool isdrawn)
{
	this->isDrawBoundingBox = isdrawn;
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

mat4 Object::getRotationMatrix() const
{
	return matRotation;
}

Material* Object::getMaterial()
{ 
	return &material;
}

bool Object::getIsDrawBoundingBox() const
{
	return isDrawBoundingBox;
}

const vector<vec3>* Object::getFaceNormals() const
{
	return &faceNormals;
}