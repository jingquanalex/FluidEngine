#pragma once

#include "Includes.h"
#include "Chunk.h"
#include "Shader.h"

class ChunkManager
{

private:

	std::unordered_map<glm::ivec3, Chunk*, KeyHash> chunks;

	Shader* shader;
	float maxHeight;

	SectionBuffer sBuffer;

	glm::ivec3 oldPos;
	glm::ivec3 voxelRange;
	glm::ivec3 meshRange;

public:

	ChunkManager();
	~ChunkManager();

	void load(std::string mapname, glm::ivec3 sectionsize, glm::ivec3 chunksize, float blocksize);
	void update(glm::vec3 cameraPosition);
	void draw(GLuint envMapId);

};