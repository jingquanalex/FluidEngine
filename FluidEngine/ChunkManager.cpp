#include "ChunkManager.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

ChunkManager::ChunkManager()
{

}

ChunkManager::~ChunkManager()
{
	if (!chunks.empty())
	{
		for (pair<ivec3, Chunk*> e : chunks)
		{
			delete e.second;
		}
	}
}

// sectionsize - width of section of heightmap
void ChunkManager::load(string mapname, ivec3 sections, int chunksize, float blocksize)
{
	//glPolygonMode(GL_FRONT, GL_LINE);

	// Assume heightmaps of dimension power of twos
	int mapWidth, mapHeight, mapChannels;
	unsigned char *heightMap = SOIL_load_image((g_mediaDirectory + mapname).c_str(),
		&mapWidth, &mapHeight, &mapChannels, SOIL_LOAD_L);

	// Flip Y
	/*for (int j = 0; j * 2 < mapWidth; ++j)
	{
		int index1 = j * mapWidth;
		int index2 = (mapWidth - 1 - j) * mapWidth;
		for (int i = mapWidth; i > 0; --i)
		{
			GLubyte temp = heightMap[index1];
			heightMap[index1] = heightMap[index2];
			heightMap[index2] = temp;
			++index1;
			++index2;
		}
	}*/

	for (int i = 0; i < sections.x; i++)
	{
		for (int j = 0; j < sections.x; j++)
		{
			for (int k = 0; k < sections.z; k++)
			{
				Chunk* chunk = new Chunk();
				Section sectionBuffer(ivec3(i, j, k), sections, mapWidth);
				chunk->load(heightMap, sectionBuffer, chunksize, blocksize);
				chunk->setPosition(vec3(
					i * chunksize * blocksize,
					k * chunksize * blocksize,
					j * chunksize * blocksize));
				chunks.insert(make_pair(ivec3(i, k, j), chunk));
			}
		}
	}

	SOIL_free_image_data(heightMap);
}

void ChunkManager::draw(GLuint envMapId)
{
	for (pair<ivec3, Chunk*> e : chunks)
	{
		e.second->draw(envMapId);
	}
}