#pragma once

#include "Includes.h"
#include "Chunk.h"
#include "Shader.h"
#include "Object.h"

class ChunkManager
{

private:

	Shader* shader;
	float maxHeight;

	std::unordered_map<glm::ivec3, Chunk*, KeyHash> chunks;
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

	const std::unordered_map<glm::ivec3, Chunk*, KeyHash>* getSections() const;
	const SectionBuffer* getSectionBuffer() const;

};