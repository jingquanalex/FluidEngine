#include "Quad.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Quad::Quad(vec3 position) : Object(position)
{

}

Quad::~Quad()
{

}

void Quad::load(string shadername)
{
	shader = new Shader(shadername);

	float quadVertices[30] =
	{
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f
	};

	// Quad vbo
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glBindVertexArray(0);
}

void Quad::load(string mapname, string shadername)
{
	load(shadername);
	loadCubemap(mapname);
}

void Quad::loadCubemap(std::string mapname)
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);

	// Load cubemap textures
	texCubeId = SOIL_load_OGL_cubemap(
		(g_mediaDirectory + mapname + "/posx.jpg").c_str(),
		(g_mediaDirectory + mapname + "/negx.jpg").c_str(),
		(g_mediaDirectory + mapname + "/posy.jpg").c_str(),
		(g_mediaDirectory + mapname + "/negy.jpg").c_str(),
		(g_mediaDirectory + mapname + "/posz.jpg").c_str(),
		(g_mediaDirectory + mapname + "/negz.jpg").c_str(),
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
		);

	if (texCubeId == 0)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
		return;
	}

	listTexCubeId.push_back(texCubeId);

	glBindTexture(GL_TEXTURE_CUBE_MAP, texCubeId);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Quad::draw()
{
	glUseProgram(shader->getProgram());
	glBindTexture(GL_TEXTURE_CUBE_MAP, listTexCubeId.at(currentTexIndex));
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

// Changed: Light* to GLuint, maybe bug
void Quad::draw(GLuint depthMapId, Camera* camera)
{
	glUseProgram(shader->getProgram());
	if (camera != nullptr)
	{
		glUniform1f(glGetUniformLocation(shader->getProgram(), "nearPlane"), camera->getNearPlane());
		glUniform1f(glGetUniformLocation(shader->getProgram(), "farPlane"), camera->getFarPlane());
		glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "invProj"),
			1, GL_FALSE, value_ptr(camera->getMatInvProjection()));
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMapId);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Quad::cycleNextMap()
{
	currentTexIndex = ++currentTexIndex >= (int)listTexCubeId.size() ? 0 : currentTexIndex;
}

void Quad::cyclePreviousMap()
{
	currentTexIndex = --currentTexIndex <= -1 ? (int)listTexCubeId.size() - 1 : currentTexIndex;
}

GLuint Quad::getCubeMap() const
{
	return listTexCubeId.at(currentTexIndex);
}

GLuint Quad::getVao() const
{
	return vao;
}