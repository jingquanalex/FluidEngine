#pragma once

#include "Includes.h"
#include "Object.h"
#include "Block.h"
#include <unordered_map>

struct Section
{
	Section(glm::ivec3 a, glm::ivec3 b, glm::ivec2 c) :
		positionIndex(a), sectionDimension(b), mapDimension(c) { }

	glm::ivec3 positionIndex;
	glm::ivec3 sectionDimension;
	glm::ivec2 mapDimension;
};

class Chunk
{

private:

	struct Vertex
	{
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texcoord, float occlusion) :
			Position(position), Normal(normal), 
			TexCoords(texcoord), Occlusion(occlusion) { }

		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		float Occlusion;
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
	bool isempty, hasloaded;

	void addFace(glm::vec3 pos, float size, CubeFace face, glm::vec4 occ);
	float findAOFactor(bool side1, bool side2, bool corner);

public:

	Chunk();
	~Chunk();

	void load(unsigned char *heightMap, Section sBuffer, glm::ivec3 chunksize, float blocksize);
	void draw();

	void setPosition(glm::vec3 position);
	glm::mat4 getModelMatrix() const;
};