#include "ChunkManager.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

ChunkManager::ChunkManager()
{
	oldPos = ivec3(-1);
	searchSize = ivec3(1);
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
void ChunkManager::load(string mapname, ivec3 sectionsize, ivec3 chunksize, float blocksize)
{
	//glPolygonMode(GL_FRONT, GL_LINE);

	shader = new Shader("terrain");
	maxHeight = sectionsize.z * chunksize.z * blocksize;

	// Assume heightmaps of dimension power of twos
	int mapWidth, mapHeight, mapChannels;
	sBuffer.HeightMap = SOIL_load_image((g_mediaDirectory + mapname).c_str(),
		&mapWidth, &mapHeight, &mapChannels, SOIL_LOAD_L);

	sBuffer.MapDimension = ivec2(mapWidth, mapHeight);
	sBuffer.SectionSize = sectionsize;
	sBuffer.ChunkSize = chunksize;
	sBuffer.BlockSize = blocksize;

	// Load voxels from heightmap
	for (int i = 0; i < sectionsize.x; i++)
	{
		for (int j = 0; j < sectionsize.y; j++)
		{
			for (int k = 0; k < sectionsize.z; k++)
			{
				// Convert to opengl coordinates to store block positions as keys
				Chunk* chunk = new Chunk(chunks);
				sBuffer.SectionIndex = ivec3(i, k, j);
				chunk->load(sBuffer);
				chunk->setPosition(vec3(
					i * chunksize.x * blocksize,
					k * chunksize.z * blocksize,
					j * chunksize.y * blocksize));
				chunks.insert(make_pair(ivec3(i, k, j), chunk));
			}
		}
	}

	// Mesh voxels
	for (pair<ivec3, Chunk*> e : chunks)
	{
		e.second->mesh();
	}

	//SOIL_free_image_data(heightMap);
}

void ChunkManager::update(vec3 cameraPosition)
{
	return;
	ivec3 localPosition = vec3(
		cameraPosition.x / sBuffer.ChunkSize.x,
		cameraPosition.z / sBuffer.ChunkSize.z,
		cameraPosition.y / sBuffer.ChunkSize.y) / sBuffer.BlockSize;

	if (localPosition != oldPos)
	{
		oldPos = localPosition;

		for (int i = -searchSize.x; i <= searchSize.x; i++)
		{
			for (int j = -searchSize.y; j <= searchSize.y; j++)
			{
				for (int k = -searchSize.z; k <= searchSize.z; k++)
				{
					ivec3 sectionPos = localPosition + ivec3(i, j, k);
					sectionPos = clamp(sectionPos, ivec3(0), sBuffer.SectionSize - ivec3(1));

					if (chunks.find(sectionPos) == chunks.end())
					{
						Chunk* chunk = new Chunk(chunks);
						sBuffer.SectionIndex = sectionPos;
						chunk->load(sBuffer);
						chunk->setPosition(vec3(
							sectionPos.x * sBuffer.ChunkSize.x * sBuffer.BlockSize,
							sectionPos.z * sBuffer.ChunkSize.z * sBuffer.BlockSize,
							sectionPos.y * sBuffer.ChunkSize.y * sBuffer.BlockSize));
						chunks.insert(make_pair(sectionPos, chunk));
						cout << sectionPos.x << ", " << sectionPos.y << ", " << sectionPos.z << endl;
					}
				}
			}
		}
	}
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