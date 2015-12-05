#include "ChunkManager.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

ChunkManager::ChunkManager()
{
	oldPos = ivec3(-999);
	voxelRange = ivec3(2);
	meshRange = ivec3(1);
}

ChunkManager::~ChunkManager()
{
	SOIL_free_image_data(sBuffer.HeightMap);

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

	// Load starting voxels from heightmap
	/*for (int i = 0; i <= sectionsize.x / 2; i++)
	{
		for (int j = 0; j <= sectionsize.y / 2; j++)
		{
			for (int k = 0; k <= sectionsize.z / 2; k++)
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
	}*/
}

void ChunkManager::update(vec3 cameraPosition)
{
	// Load voxels and build mesh for those chunk around the camera
	ivec3 localPosition = vec3(
		cameraPosition.x / sBuffer.ChunkSize.x,
		cameraPosition.y / sBuffer.ChunkSize.z,
		cameraPosition.z / sBuffer.ChunkSize.y) / sBuffer.BlockSize;

	if (localPosition != oldPos)
	{
		// Search all surrounding chunk neighbors of range voxelRange
		for (int i = -voxelRange.x; i <= voxelRange.x; i++)
		{
			for (int j = -voxelRange.y; j <= voxelRange.y; j++)
			{
				for (int k = -voxelRange.z; k <= voxelRange.z; k++)
				{
					// Find which chunk camera is in
					ivec3 sectionPos = localPosition + ivec3(i, j, k);
					sectionPos = clamp(sectionPos, ivec3(0), ivec3(
						sBuffer.SectionSize.x, sBuffer.SectionSize.z, sBuffer.SectionSize.y) - ivec3(1));
					
					// Populate voxel data in that chunk
					if (chunks.find(sectionPos) == chunks.end())
					{
						Chunk* chunk = new Chunk(chunks);
						sBuffer.SectionIndex = sectionPos;
						chunk->load(&sBuffer);
						chunk->setPosition(vec3(
							sectionPos.x * sBuffer.ChunkSize.x * sBuffer.BlockSize,
							sectionPos.y * sBuffer.ChunkSize.z * sBuffer.BlockSize,
							sectionPos.z * sBuffer.ChunkSize.y * sBuffer.BlockSize));
						chunks.insert(make_pair(sectionPos, chunk));
						cout << "voxel: " 
							<< sectionPos.x << ", " << sectionPos.y << ", " << sectionPos.z << endl;
					}
				}
			}
		}

		// Search all surrounding chunk neighbors of range meshRange
		for (int i = -meshRange.x; i <= meshRange.x; i++)
		{
			for (int j = -meshRange.y; j <= meshRange.y; j++)
			{
				for (int k = -meshRange.z; k <= meshRange.z; k++)
				{
					ivec3 sectionPos = localPosition + ivec3(i, j, k);
					sectionPos = clamp(sectionPos, ivec3(0), ivec3(
						sBuffer.SectionSize.x, sBuffer.SectionSize.z, sBuffer.SectionSize.y) - ivec3(1));

					// Mesh that chunk if not done
					auto it = chunks.find(sectionPos);
					if (it != chunks.end() && !it->second->getIsEmpty() && !it->second->getIsMeshed())
					{
						it->second->mesh();
						cout << "mesh: " 
							<< sectionPos.x << ", " << sectionPos.y << ", "  << sectionPos.z << endl;
					}
				}
			}
		}

		oldPos = localPosition;
	}
}

void ChunkManager::draw(GLuint envMapId, Light* light)
{
	GLuint program = shader->getProgram();
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "diffuse1"), 0);
	glUniform1i(glGetUniformLocation(program, "cubemap1"), 1);
	glUniform1f(glGetUniformLocation(program, "maxheight"), maxHeight);

	if (light != nullptr)
	{
		glUniformMatrix4fv(11, 1, GL_FALSE, value_ptr(light->getMatLight()));
		glUniform1i(glGetUniformLocation(program, "shadowmap1"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, light->getDepthMap());
	}

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

void ChunkManager::draw(Light* light)
{
	glUseProgram(light->getProgram());
	glUniformMatrix4fv(11, 1, GL_FALSE, value_ptr(light->getMatLight()));
	for (pair<ivec3, Chunk*> e : chunks)
	{
		glUniformMatrix4fv(10, 1, GL_FALSE, value_ptr(e.second->getModelMatrix()));
		e.second->draw();
	}
}

const std::unordered_map<glm::ivec3, Chunk*, KeyHash>* ChunkManager::getSections() const
{
	return &chunks;
}

const SectionBuffer* ChunkManager::getSectionBuffer() const
{
	return &sBuffer;
}