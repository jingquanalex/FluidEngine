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
void ChunkManager::load(string mapname, ivec3 sections, ivec3 chunksize, float blocksize)
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
		for (int j = 0; j < sections.y; j++)
		{
			for (int k = 0; k < sections.z; k++)
			{
				Chunk* chunk = new Chunk();
				Section sBuffer(ivec3(i, j, k), sections, ivec2(mapWidth, mapHeight));
				chunk->load(heightMap, sBuffer, chunksize, blocksize);
				chunk->setPosition(vec3(
					i * chunksize.x * blocksize,
					k * chunksize.z * blocksize,
					j * chunksize.y * blocksize));
				chunks.insert(make_pair(ivec3(i, k, j), chunk));
			}
		}
	}

	SOIL_free_image_data(heightMap);

	shader = new Shader("terrain");
	maxHeight = sections.z * chunksize.z * blocksize;
}

void ChunkManager::draw(GLuint envMapId)
{
	GLuint program = shader->getProgram();
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "diffuse1"), 0);
	glUniform1i(glGetUniformLocation(program, "cubemap1"), 1);
	glUniform1f(glGetUniformLocation(program, "maxheight"), maxHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envMapId);

	for (pair<ivec3, Chunk*> e : chunks)
	{
		glUniformMatrix4fv(10, 1, GL_FALSE, value_ptr(e.second->getModelMatrix()));
		e.second->draw();
	}
}