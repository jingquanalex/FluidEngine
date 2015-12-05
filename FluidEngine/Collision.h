#pragma once

#include "Includes.h"
#include "ChunkManager.h"
#include "Airplane.h"

class Collision
{

public:

	Collision();
	~Collision();

	bool isPointInTerrain(ChunkManager* chunkMgr, glm::vec3 point);
	void resolveTerrainObject(ChunkManager* chunkMgr, Object* obj);
	void resolveTerrainPlane(ChunkManager* chunkMgr, Airplane* plane);
	void resolveObjectSAT(Object* obj1, Object* obj2);
	
};