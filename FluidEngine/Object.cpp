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

void Object::load(string shadername)
{
	// Make cube data vectors
	vector<Vertex> listVertices;
	for (int i = 0; i < 8; i++)
	{
		Vertex vertex;
		vertex.Position = vec3(cubeVertices[i * 3 + 0], cubeVertices[i * 3 + 1], cubeVertices[i * 3 + 2]);
		vertex.Normal = vec3();
		vertex.TexCoords = vec2(cubeTexCoords[i * 2 + 0], cubeTexCoords[i * 2 + 1]);
		listVertices.push_back(vertex);
	}
	vector<GLuint> listIndices(cubeIndices, cubeIndices + 36);
	vector<Texture> listTextures;

	mesh = new Mesh(listVertices, listIndices, listTextures);
	shader = new Shader(shadername);
}

void Object::update(float dt, Camera* camera)
{
	// Apply object transformations
	matModel = translate(position);
	shader->update(dt, camera);
}

void Object::draw()
{
	glUseProgram(shader->getProgram());
	glUniformMatrix4fv(10, 1, GL_FALSE, value_ptr(matModel));

	mesh->draw();

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


const GLfloat Object::triVertices[] =
{
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};

const GLuint Object::triIndices[] =
{
	0, 1, 2
};

// Cube vertices, color, texcoord
const GLfloat Object::cubeVertices[] =
{
	// Front
	-0.5, -0.5, 0.5,
	0.5, -0.5, 0.5,
	0.5, 0.5, 0.5,
	-0.5, 0.5, 0.5,

	// Back
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5, 0.5, -0.5,
	-0.5, 0.5, -0.5
};

const GLfloat Object::cubeTexCoords[] =
{
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,

	1.0, 0.0,
	0.0, 0.0,
	0.0, 1.0,
	1.0, 1.0
};

const GLuint Object::cubeIndices[] =
{
	// Front
	0, 1, 2,
	2, 3, 0,
	// Top
	3, 2, 6,
	6, 7, 3,
	// Back
	7, 6, 5,
	5, 4, 7,
	// Bottom
	4, 5, 1,
	1, 0, 4,
	// Left
	4, 0, 3,
	3, 7, 4,
	// Right
	1, 5, 6,
	6, 2, 1
};