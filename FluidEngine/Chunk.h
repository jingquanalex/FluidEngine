#pragma once

#include "Includes.h"
#include "Object.h"
#include "Block.h"
#include <unordered_map>

struct Section
{
	Section(glm::ivec3 a, glm::ivec3 b, int c) :
		index(a), sections(b), mapwidth(c) { }

	glm::ivec3 index;
	glm::ivec3 sections;
	int mapwidth;
};

class Chunk : public Object
{

private:

	struct Vertex
	{
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texcoord) :
			Position(position), Normal(normal), TexCoords(texcoord) { }

		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	enum class CubeFace { Top, Bottom, Left, Right, Front, Back };

	struct KeyHash
	{
		size_t operator()(const glm::ivec3& key) const
		{
			return ((key.x * 73856093) ^ (key.y * 19349663)) ^ (key.z * 83492791);
		}
	};

	std::unordered_map<glm::ivec3, Block, KeyHash> blocks;

	std::vector<Vertex> vertices;
	int verticesSize;
	GLuint vbo;
	GLuint vao;
	glm::mat4 matModel;
	bool isempty;

	void addFace(glm::vec3 pos, float size, CubeFace face);

public:

	Chunk();
	~Chunk();

	void load(unsigned char *heightMap, Section sectionBuffer, int chunksize, float blocksize);
	void draw(GLuint envMapId);

	void setPosition(glm::vec3 position);
};