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

void Quad::load(string mapname, string shadername)
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);

	shader = new Shader(shadername);

	float quadVertices[18] = 
	{
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f 
	};

	// Quad vbo
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glBindVertexArray(0);

	// Load cubemap textures
	texid = SOIL_load_OGL_cubemap(
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

	if (texid == 0)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, texid);
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
	glBindTexture(GL_TEXTURE_CUBE_MAP, texid);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

GLuint Quad::getCubeMapID() const
{
	return texid;
}