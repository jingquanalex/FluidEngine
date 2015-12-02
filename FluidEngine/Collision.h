#pragma once

#include "Includes.h"
#include "ChunkManager.h"
#include "Object.h"

class Collision
{

public:

	Collision();
	~Collision();

	void resolveTerrainObject(ChunkManager* chunkMgr, Object* obj);
};