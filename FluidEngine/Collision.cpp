#include "Collision.h"

using namespace glm;
using namespace std;

Collision::Collision()
{

}

Collision::~Collision()
{

}

// Check if object's bounding boxes are colliding with terrain
void Collision::resolveTerrainObject(ChunkManager* chunkMgr, Object* obj)
{
	const SectionBuffer* sBuffer = chunkMgr->getSectionBuffer();
	const unordered_map<ivec3, Chunk*, KeyHash>* chunks = chunkMgr->getSections();

	for (auto itbb = obj->getBoundingBoxList()->begin(); itbb != obj->getBoundingBoxList()->end(); itbb++)
	{
		// Point test: For each vertex of the object's bounding boxes
		for (auto itv = itbb->Vertices.begin(); itv != itbb->Vertices.end(); itv++)
		{
			vec3 localPos = vec3(
				itv->x / sBuffer->ChunkSize.x,
				itv->y / sBuffer->ChunkSize.z,
				itv->z / sBuffer->ChunkSize.y) / sBuffer->BlockSize;

			// Find its position in the sections
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

				if (its->second->hasBlock(blockPos))
				{
					obj->getMaterial()->setDiffuseColor(vec3(0.9, 0.3, 0.2));
					return;
				}
			}
		}
	}

	obj->getMaterial()->setDiffuseColor(vec3(0.3, 0.5, 0.8));
}