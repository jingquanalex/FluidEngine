#pragma once

#include "globalIncludes.h"
#include <string>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	GLuint id;
	std::string type;
	std::string path;
};

class Mesh
{

private:

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	GLuint vbo;
	GLuint vao;
	GLuint ebo;

public:

	Mesh(const std::vector<Vertex>& vertices, 
		const std::vector<GLuint>& indices, 
		const std::vector<Texture>& textures);
	~Mesh();

	void draw();
	void bindTexture(GLuint program);

};