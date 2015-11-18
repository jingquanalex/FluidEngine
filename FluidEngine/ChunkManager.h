#pragma once

#include "Includes.h"
#include "Chunk.h"
#include <unordered_map>

class ChunkManager
{

private:

	struct KeyHash
	{
		size_t operator()(const glm::ivec3& key) const
		{
			return ((key.x * 73856093) ^ (key.y * 19349663)) ^ (key.z * 83492791);
		}
	};

	std::unordered_map<glm::ivec3, Chunk*, KeyHash> chunks;

public:

	ChunkManager();
	~ChunkManager();

	void load(std::string mapname, glm::ivec3 sections, int chunksize, float blocksize);
	void draw();

};