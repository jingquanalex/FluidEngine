#pragma once

#include "Includes.h"
#include "Object.h"
#include "Block.h"
#include <unordered_map>

struct KeyHash
{
	size_t operator()(const glm::ivec3& key) const
	{
		return ((key.x * 73856093) ^ (key.y * 19349663)) ^ (key.z * 83492791);
	}
};

struct SectionBuffer
{
	SectionBuffer() { }
	SectionBuffer(
		unsigned char* heightmap, glm::ivec2 mapdimension, glm::ivec3 sectionindex, 
		glm::ivec3 sectionsize, glm::ivec3 chunksize, float blocksize) :
		HeightMap(heightmap), MapDimension(mapdimension), SectionIndex(sectionindex), 
		SectionSize(sectionsize), ChunkSize(chunksize), BlockSize(blocksize) { }

	unsigned char* HeightMap;
	glm::ivec2 MapDimension;
	glm::ivec3 SectionIndex;
	glm::ivec3 SectionSize;
	glm::ivec3 ChunkSize;
	float BlockSize;
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

	std::unordered_map<glm::ivec3, Block, KeyHash> blocks;
	const std::unordered_map<glm::ivec3, Chunk*, KeyHash>* chunks;

	std::vector<Vertex> vertices;
	int verticesSize;
	GLuint vbo;
	GLuint vao;
	glm::mat4 matModel;
	bool isEmpty, isMeshed;
	glm::ivec3 sectionIndex;
	SectionBuffer sBuffer;

	void addFace(glm::vec3 pos, float size, CubeFace face, glm::vec4 occ);
	float findAOFactor(bool side1, bool side2, bool corner);
	bool findNeighbor(glm::ivec3 index);
	bool findNeighborInChunk(glm::ivec3 index);

public:

	Chunk(const std::unordered_map<glm::ivec3, Chunk*, KeyHash>& chunks);
	~Chunk();

	void load(SectionBuffer sBuffer);
	void mesh();
	void draw();

	void setPosition(glm::vec3 position);
	glm::mat4 getModelMatrix() const;
};