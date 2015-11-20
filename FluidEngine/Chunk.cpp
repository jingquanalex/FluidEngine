#include "Chunk.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Chunk::Chunk(const unordered_map<ivec3, Chunk*, KeyHash>& chunks)
{
	this->chunks = &chunks;
	matModel = mat4();
	isEmpty = false;
	isMeshed = false;
}

Chunk::~Chunk()
{
	
}

// Build the 3D array of blocks of size in all dimension
// sBuffer(x index, y index, section width)
void Chunk::load(SectionBuffer sBuffer)
{
	this->sBuffer = sBuffer;
	blocks.reserve(sBuffer.ChunkSize.x * sBuffer.ChunkSize.y * sBuffer.ChunkSize.z);

	// Generate chunk of blocks based on a section of heightmap
	// Image Coords
	for (int i = 0; i < sBuffer.ChunkSize.x; i++)
	{
		for (int j = 0; j < sBuffer.ChunkSize.y; j++)
		{
			// Sample depth value of chunk[i, j] at section[indexX, indexY] of heightmap
			int sectionWidth = sBuffer.MapDimension.x / sBuffer.SectionSize.x;
			int sectionHeight = sBuffer.MapDimension.y / sBuffer.SectionSize.y;
			int cellX = sectionWidth / sBuffer.ChunkSize.x;
			int cellY = sectionHeight / sBuffer.ChunkSize.y;
			int indexX = cellX / 2 + cellX * i + sBuffer.SectionIndex.x * sectionWidth;
			int indexY = cellY / 2 + cellY * j + sBuffer.SectionIndex.z * sectionWidth;
			unsigned char depthValue = sBuffer.HeightMap[indexX + indexY * sBuffer.MapDimension.x];

			// Generate block if posValue lower than sampled depth value
			int sectionDepth = 256 / sBuffer.SectionSize.z;
			int cellZ = sectionDepth / sBuffer.ChunkSize.z;
			for (int k = 0; k < sBuffer.ChunkSize.z; k++)
			{
				int posValue = cellZ / 2 + cellZ * k + sBuffer.SectionIndex.y * sectionDepth;
				if (posValue < depthValue)
				{
					blocks.insert(make_pair(ivec3(i, k, j), Block()));
				}
			}
		}
	}

	isEmpty = blocks.empty();
}

void Chunk::mesh()
{
	if (isEmpty) return;

	// Create mesh for each chunk
	vec4 occlusionFactor;
	float topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor;
	float blockHalfSize = sBuffer.BlockSize / 2;
	for (pair<ivec3, Block> e : blocks)
	{
		// If no neighbor block present, make a face there
		if (findNeighbor(e.first + ivec3(1, 0, 0)) == false)
		{
			// Search block neighbors to find occlusion factor for all 4 face vertices
			topLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, 1, 0)),
				findNeighbor(e.first + ivec3(1, 0, 1)),
				findNeighbor(e.first + ivec3(1, 1, 1)));
			bottomLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, 0, 1)),
				findNeighbor(e.first + ivec3(1, -1, 0)),
				findNeighbor(e.first + ivec3(1, -1, 1)));
			bottomRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, 0, -1)),
				findNeighbor(e.first + ivec3(1, -1, 0)),
				findNeighbor(e.first + ivec3(1, -1, -1)));
			topRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, 1, 0)),
				findNeighbor(e.first + ivec3(1, 0, -1)),
				findNeighbor(e.first + ivec3(1, 1, -1)));
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Right, occlusionFactor);
		}
		if (findNeighbor(e.first + ivec3(-1, 0, 0)) == false)
		{
			topLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, 1, 0)),
				findNeighbor(e.first + ivec3(-1, 0, -1)),
				findNeighbor(e.first + ivec3(-1, 1, -1)));
			bottomLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, 0, -1)),
				findNeighbor(e.first + ivec3(-1, -1, 0)),
				findNeighbor(e.first + ivec3(-1, -1, -1)));
			bottomRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, 0, 1)),
				findNeighbor(e.first + ivec3(-1, -1, 0)),
				findNeighbor(e.first + ivec3(-1, -1, 1)));
			topRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, 1, 0)),
				findNeighbor(e.first + ivec3(-1, 0, 1)),
				findNeighbor(e.first + ivec3(-1, 1, 1)));
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Left, occlusionFactor);
		}
		if (findNeighbor(e.first + ivec3(0, 1, 0)) == false)
		{
			topLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, 1, 0)),
				findNeighbor(e.first + ivec3(0, 1, -1)),
				findNeighbor(e.first + ivec3(-1, 1, -1)));
			bottomLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, 1, 0)),
				findNeighbor(e.first + ivec3(0, 1, 1)),
				findNeighbor(e.first + ivec3(-1, 1, 1)));
			bottomRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, 1, 0)),
				findNeighbor(e.first + ivec3(0, 1, 1)),
				findNeighbor(e.first + ivec3(1, 1, 1)));
			topRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, 1, 0)),
				findNeighbor(e.first + ivec3(0, 1, -1)),
				findNeighbor(e.first + ivec3(1, 1, -1)));
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Top, occlusionFactor);
		}
		if (findNeighbor(e.first + ivec3(0, -1, 0)) == false)
		{
			topLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, -1, 0)),
				findNeighbor(e.first + ivec3(0, -1, 1)),
				findNeighbor(e.first + ivec3(-1, -1, 1)));
			bottomLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, -1, 0)),
				findNeighbor(e.first + ivec3(0, -1, -1)),
				findNeighbor(e.first + ivec3(-1, -1, -1)));
			bottomRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, -1, 0)),
				findNeighbor(e.first + ivec3(0, -1, -1)),
				findNeighbor(e.first + ivec3(1, -1, -1)));
			topRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, -1, 0)),
				findNeighbor(e.first + ivec3(0, -1, 1)),
				findNeighbor(e.first + ivec3(1, -1, 1)));
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Bottom, occlusionFactor);
		}
		if (findNeighbor(e.first + ivec3(0, 0, 1)) == false)
		{
			topLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, 0, 1)),
				findNeighbor(e.first + ivec3(0, 1, 1)),
				findNeighbor(e.first + ivec3(-1, 1, 1)));
			bottomLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, 0, 1)),
				findNeighbor(e.first + ivec3(0, -1, 1)),
				findNeighbor(e.first + ivec3(-1, -1, 1)));
			bottomRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, 0, 1)),
				findNeighbor(e.first + ivec3(0, -1, 1)),
				findNeighbor(e.first + ivec3(1, -1, 1)));
			topRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, 0, 1)),
				findNeighbor(e.first + ivec3(0, 1, 1)),
				findNeighbor(e.first + ivec3(1, 1, 1)));
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Front, occlusionFactor);
		}
		if (findNeighbor(e.first + ivec3(0, 0, -1)) == false)
		{
			topLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, 0, -1)),
				findNeighbor(e.first + ivec3(0, 1, -1)),
				findNeighbor(e.first + ivec3(1, 1, -1)));
			bottomLeftFactor = findAOFactor(
				findNeighbor(e.first + ivec3(1, 0, -1)),
				findNeighbor(e.first + ivec3(0, -1, -1)),
				findNeighbor(e.first + ivec3(1, -1, -1)));
			bottomRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, 0, -1)),
				findNeighbor(e.first + ivec3(0, -1, -1)),
				findNeighbor(e.first + ivec3(-1, -1, -1)));
			topRightFactor = findAOFactor(
				findNeighbor(e.first + ivec3(-1, 0, -1)),
				findNeighbor(e.first + ivec3(0, 1, -1)),
				findNeighbor(e.first + ivec3(-1, 1, -1)));
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Back, occlusionFactor);
		}
	}

	// All faces culled, chunk is considered empty
	if (vertices.size() == 0)
	{
		isEmpty = true;
		return;
	}

	// Send vertices to GPU
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::TexCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::Occlusion));
	glBindVertexArray(0);

	verticesSize = vertices.size();
	vertices.clear();

	isMeshed = true;
}

// Calculate the occlusion factor based on the presence of left, right, and corner neighbor voxel
// at a given vertex.
float Chunk::findAOFactor(bool side1, bool side2, bool corner)
{
	int level = 0;
	if (side1 && side2)
		level = 0;
	else
		level = 3 - (side1 + side2 + corner);

	if (level == 3)
		level = level;

	return 0.7f + level * 0.1f;
}

// Find local neighbor
bool Chunk::findNeighbor(ivec3 index)
{
	bool hasLocalNeighbor = blocks.find(index) != blocks.end();
	if (!hasLocalNeighbor)
	{
		return findNeighborInChunk(index);
	}
	return hasLocalNeighbor;
}

// Find local neighbor in neighboring chunk
bool Chunk::findNeighborInChunk(ivec3 index)
{
	ivec3 chunkIndex, newIndex = index;
	
	// Search in chunkIndex for block at newIndex
	// NOTE: ChunkSize is in image coord
	if (index.x == sBuffer.ChunkSize.x) // Right
	{
		chunkIndex = sBuffer.SectionIndex + ivec3(1, 0, 0);
		newIndex.x = 0;
	}
	else if (index.x == -1) // Left
	{
		chunkIndex = sBuffer.SectionIndex + ivec3(-1, 0, 0);
		newIndex.x = sBuffer.ChunkSize.x - 1;
	}
	else if (index.y == sBuffer.ChunkSize.z) // Top
	{
		chunkIndex = sBuffer.SectionIndex + ivec3(0, 1, 0);
		newIndex.y = 0;
	}
	else if (index.y == -1) // Bottom
	{
		chunkIndex = sBuffer.SectionIndex + ivec3(0, -1, 0);
		newIndex.y = sBuffer.ChunkSize.z - 1;
	}
	else if (index.z == sBuffer.ChunkSize.y) // Front
	{
		chunkIndex = sBuffer.SectionIndex + ivec3(0, 0, 1);
		newIndex.z = 0;
	}
	else if (index.z == -1) // Back
	{
		chunkIndex = sBuffer.SectionIndex + ivec3(0, 0, -1);
		newIndex.z = sBuffer.ChunkSize.y - 1;
	}
	else
	{
		return false;
	}

	auto it = chunks->find(chunkIndex);
	if (it != chunks->end())
	{
		return it->second->findNeighbor(newIndex);
	}

	return false;
}

// Make a face of a cube at center [pos] with square width of [size * 2]
// pos - cube center, size - half width (or height) of cube, face - which face to generate
// occ - set the occlusion factor for the 4 vertices of the face, [topleft,botleft,botright,topright]
void Chunk::addFace(vec3 pos, float size, CubeFace face, vec4 occ)
{
	float offset = size * 2;
	vec3 v1, v2, v3, v4;

	switch (face)
	{
	case CubeFace::Front:
	{
		v1 = vec3(-size, size, size);
		v2 = vec3(-size, -size, size);
		v3 = vec3(size, -size, size);
		v4 = vec3(size, size, size);

		if (occ.x + occ.z > occ.y + occ.w)
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, 0.0, 1.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, 0.0, 1.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, 0.0, 1.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, 0.0, 1.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, 0.0, 1.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, 0.0, 1.0), vec2(0.0, 1.0), occ.x));
		}
		else
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, 0.0, 1.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, 0.0, 1.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, 0.0, 1.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, 0.0, 1.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, 0.0, 1.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, 0.0, 1.0), vec2(1.0, 0.0), occ.z));
		}

		break;
	}
	case CubeFace::Back:
	{
		v1 = vec3(size, size, -size);
		v2 = vec3(size, -size, -size);
		v3 = vec3(-size, -size, -size);
		v4 = vec3(-size, size, -size);

		if (occ.x + occ.z > occ.y + occ.w)
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, 0.0, -1.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, 0.0, -1.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, 0.0, -1.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, 0.0, -1.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, 0.0, -1.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, 0.0, -1.0), vec2(0.0, 1.0), occ.x));
		}
		else
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, 0.0, -1.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, 0.0, -1.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, 0.0, -1.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, 0.0, -1.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, 0.0, -1.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, 0.0, -1.0), vec2(1.0, 0.0), occ.z));
		}

		break;
	}
	case CubeFace::Top:
	{
		v1 = vec3(-size, size, -size);
		v2 = vec3(-size, size, size);
		v3 = vec3(size, size, size);
		v4 = vec3(size, size, -size);

		if (occ.x + occ.z > occ.y + occ.w)
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, 1.0, 0.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, 1.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, 1.0, 0.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, 1.0, 0.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, 1.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, 1.0, 0.0), vec2(0.0, 1.0), occ.x));
		}
		else
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, 1.0, 0.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, 1.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, 1.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, 1.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, 1.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, 1.0, 0.0), vec2(1.0, 0.0), occ.z));
		}

		break;
	}
	case CubeFace::Bottom:
	{
		v1 = vec3(-size, -size, size);
		v2 = vec3(-size, -size, -size);
		v3 = vec3(size, -size, -size);
		v4 = vec3(size, -size, size);

		if (occ.x + occ.z > occ.y + occ.w)
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, -1.0, 0.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, -1.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, -1.0, 0.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, -1.0, 0.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, -1.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, -1.0, 0.0), vec2(0.0, 1.0), occ.x));
		}
		else
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(0.0, -1.0, 0.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, -1.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, -1.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v4, vec3(0.0, -1.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v2, vec3(0.0, -1.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(0.0, -1.0, 0.0), vec2(1.0, 0.0), occ.z));
		}

		break;
	}
	case CubeFace::Left:
	{
		v1 = vec3(-size, size, -size);
		v2 = vec3(-size, -size, -size);
		v3 = vec3(-size, -size, size);
		v4 = vec3(-size, size, size);

		if (occ.x + occ.z > occ.y + occ.w)
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(-1.0, 0.0, 0.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(-1.0, 0.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(-1.0, 0.0, 0.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v3, vec3(-1.0, 0.0, 0.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v4, vec3(-1.0, 0.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v1, vec3(-1.0, 0.0, 0.0), vec2(0.0, 1.0), occ.x));
		}
		else
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(-1.0, 0.0, 0.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(-1.0, 0.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v4, vec3(-1.0, 0.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v4, vec3(-1.0, 0.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v2, vec3(-1.0, 0.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(-1.0, 0.0, 0.0), vec2(1.0, 0.0), occ.z));
		}

		break;
	}
	case CubeFace::Right:
	{
		v1 = vec3(size, size, size);
		v2 = vec3(size, -size, size);
		v3 = vec3(size, -size, -size);
		v4 = vec3(size, size, -size);

		if (occ.x + occ.z > occ.y + occ.w)
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(1.0, 0.0, 0.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(1.0, 0.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(1.0, 0.0, 0.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v3, vec3(1.0, 0.0, 0.0), vec2(1.0, 0.0), occ.z));
			vertices.push_back(Vertex(pos * offset + v4, vec3(1.0, 0.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v1, vec3(1.0, 0.0, 0.0), vec2(0.0, 1.0), occ.x));
		}
		else
		{
			vertices.push_back(Vertex(pos * offset + v1, vec3(1.0, 0.0, 0.0), vec2(0.0, 1.0), occ.x));
			vertices.push_back(Vertex(pos * offset + v2, vec3(1.0, 0.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v4, vec3(1.0, 0.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v4, vec3(1.0, 0.0, 0.0), vec2(1.0, 1.0), occ.w));
			vertices.push_back(Vertex(pos * offset + v2, vec3(1.0, 0.0, 0.0), vec2(0.0, 0.0), occ.y));
			vertices.push_back(Vertex(pos * offset + v3, vec3(1.0, 0.0, 0.0), vec2(1.0, 0.0), occ.z));
		}

		break;
	}
	}
}

// Create quad for all visible faces
void Chunk::draw()
{
	if (isEmpty || !isMeshed) return;

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, verticesSize);
	glBindVertexArray(0);
}

void Chunk::setPosition(glm::vec3 position)
{
	matModel = translate(position);
}

mat4 Chunk::getModelMatrix() const
{
	return matModel;
}