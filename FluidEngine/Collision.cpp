#include "Collision.h"

using namespace glm;
using namespace std;

Collision::Collision()
{

}

Collision::~Collision()
{

}

// Check if point is inside terrain
bool Collision::isPointInTerrain(ChunkManager* chunkMgr, vec3 point)
{
	const SectionBuffer* sBuffer = chunkMgr->getSectionBuffer();
	const unordered_map<ivec3, Chunk*, KeyHash>* chunks = chunkMgr->getSections();

	vec3 localPos = vec3(
		point.x / sBuffer->ChunkSize.x,
		point.y / sBuffer->ChunkSize.z,
		point.z / sBuffer->ChunkSize.y) / sBuffer->BlockSize;

	// Find point in the sections coordinate
	ivec3 sectionPos(localPos);
	auto its = chunks->find(sectionPos);
	if (its != chunks->end() && !its->second->getIsEmpty())
	{
		float intpart;
		vec3 localRange = vec3(
			modf(localPos.x, &intpart),
			modf(localPos.y, &intpart),
			modf(localPos.z, &intpart));

		// Offset half a block size and clamp the result
		localRange += vec3(
			1.0 / sBuffer->ChunkSize.x / 2,
			1.0 / sBuffer->ChunkSize.z / 2,
			1.0 / sBuffer->ChunkSize.y / 2);
		localRange = clamp(localRange, -0.999999f, 0.999999f);

		ivec3 blockPos = ivec3(
			localRange.x * sBuffer->ChunkSize.x,
			localRange.y * sBuffer->ChunkSize.z,
			localRange.z * sBuffer->ChunkSize.y);

		return its->second->hasBlock(blockPos);
	}

	return false;
}

// Check if object's bounding boxes vertices are colliding with terrain
void Collision::resolveTerrainObject(ChunkManager* chunkMgr, Object* obj)
{
	for (auto itbb = obj->getBoundingBoxList()->begin(); itbb != obj->getBoundingBoxList()->end(); itbb++)
	{
		for (auto itv = itbb->Vertices.begin(); itv != itbb->Vertices.end(); itv++)
		{
			if (isPointInTerrain(chunkMgr, *itv))
			{
				obj->getMaterial()->setDiffuseColor(vec3(0.9, 0.3, 0.2));
				return;
			}
		}
	}

	obj->getMaterial()->setDiffuseColor(vec3(0.3, 0.5, 0.8));
}

void Collision::resolveTerrainPlane(ChunkManager* chunkMgr, Airplane* plane)
{
	vec3 planeBottom = plane->getPosition() - vec3(0, 2, 0);
	plane->setIsAirborne(!isPointInTerrain(chunkMgr, planeBottom));

	for (auto itbb = plane->getBoundingBoxList()->begin(); itbb != plane->getBoundingBoxList()->end(); itbb++)
	{
		for (auto itv = itbb->Vertices.begin(); itv != itbb->Vertices.end(); itv++)
		{
			if (isPointInTerrain(chunkMgr, *itv))
			{
				plane->setHasCollided(true);
				plane->getMaterial()->setEmissiveColor(vec3(0.9, 0.3, 0.2));
				return;
			}
		}
	}

	plane->setHasCollided(false);
	plane->getMaterial()->setEmissiveColor(vec3(0));
}

// Detect collision of two oriented bounding box
void Collision::resolveObjectSAT(Object* obj1, Object* obj2)
{
	for (auto it1 = obj1->getBoundingBoxList()->begin(); it1 != obj1->getBoundingBoxList()->end(); it1++)
	{
		for (auto it2 = obj2->getBoundingBoxList()->begin(); it2 != obj2->getBoundingBoxList()->end(); it2++)
		{

		}
	}
}

