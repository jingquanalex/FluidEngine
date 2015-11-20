#include "Chunk.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Chunk::Chunk()
{
	matModel = mat4();
	isempty = false;
	hasloaded = false;
}

Chunk::~Chunk()
{
	
}

// Build the 3D array of blocks of size in all dimension
// sBuffer(x index, y index, section width)
void Chunk::load(unsigned char *heightMap, Section sBuffer, ivec3 chunksize, float blocksize)
{
	blocks.reserve(chunksize.x * chunksize.y * chunksize.z);

	// Generate chunk of blocks based on a section of heightmap
	for (int i = 0; i < chunksize.x; i++)
	{
		for (int j = 0; j < chunksize.y; j++)
		{
			// Sample depth value of chunk[i, j] at section[indexX, indexY] of heightmap
			int sectionWidth = sBuffer.mapDimension.x / sBuffer.sectionDimension.x;
			int sectionHeight = sBuffer.mapDimension.y / sBuffer.sectionDimension.y;
			int cellX = sectionWidth / chunksize.x;
			int cellY = sectionHeight / chunksize.y;
			int indexX = cellX / 2 + cellX * i + sBuffer.positionIndex.x * sectionWidth;
			int indexY = cellY / 2 + cellY * j + sBuffer.positionIndex.y * sectionWidth;
			unsigned char depthValue = heightMap[indexX + indexY * sBuffer.mapDimension.x];

			// Generate block if posValue lower than sampled depth value
			int sectionDepth = 256 / sBuffer.sectionDimension.z;
			int cellZ = sectionDepth / chunksize.z;
			for (int k = 0; k < chunksize.z; k++)
			{
				int posValue = cellZ / 2 + cellZ * k + sBuffer.positionIndex.z * sectionDepth;
				if (posValue < depthValue)
				{
					blocks.insert(make_pair(ivec3(i, k, j), Block()));
				}
			}
		}
	}

	isempty = blocks.empty();
	if (isempty) return;

	// Create mesh for each block
	vec4 occlusionFactor;
	float topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor;
	float blockHalfSize = blocksize / 2;
	for (pair<vec3, Block> e : blocks)
	{
		// Make face if no neighbor voxel present
		// Then search certain neighbors to find occlusion factor
		if (blocks.find(ivec3(e.first.x + 1, e.first.y, e.first.z)) == blocks.end())
		{
			topLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y + 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y + 1, e.first.z + 1)) != blocks.end());
			bottomLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y - 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y - 1, e.first.z + 1)) != blocks.end());
			bottomRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y - 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y - 1, e.first.z - 1)) != blocks.end());
			topRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y + 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y + 1, e.first.z - 1)) != blocks.end());
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Right, occlusionFactor);
		}
		if (blocks.find(ivec3(e.first.x - 1, e.first.y, e.first.z)) == blocks.end())
		{
			topLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y + 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y + 1, e.first.z - 1)) != blocks.end());
			bottomLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y - 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y - 1, e.first.z - 1)) != blocks.end());
			bottomRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y - 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y - 1, e.first.z + 1)) != blocks.end());
			topRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y + 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y + 1, e.first.z + 1)) != blocks.end());
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Left, occlusionFactor);
		}
		if (blocks.find(ivec3(e.first.x, e.first.y + 1, e.first.z)) == blocks.end())
		{
			topLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y + 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y + 1, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y + 1, e.first.z - 1)) != blocks.end());
			bottomLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y + 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y + 1, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y + 1, e.first.z + 1)) != blocks.end());
			bottomRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y + 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y + 1, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y + 1, e.first.z + 1)) != blocks.end());
			topRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y + 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y + 1, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y + 1, e.first.z - 1)) != blocks.end());
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Top, occlusionFactor);
		}
		if (blocks.find(ivec3(e.first.x, e.first.y - 1, e.first.z)) == blocks.end())
		{
			topLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y - 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y - 1, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y - 1, e.first.z + 1)) != blocks.end());
			bottomLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y - 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y - 1, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y - 1, e.first.z - 1)) != blocks.end());
			bottomRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y - 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y - 1, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y - 1, e.first.z - 1)) != blocks.end());
			topRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y - 1, e.first.z)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y - 1, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y - 1, e.first.z + 1)) != blocks.end());
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Bottom, occlusionFactor);
		}
		if (blocks.find(ivec3(e.first.x, e.first.y, e.first.z + 1)) == blocks.end())
		{
			topLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y + 1, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y + 1, e.first.z + 1)) != blocks.end());
			bottomLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y - 1, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y - 1, e.first.z + 1)) != blocks.end());
			bottomRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y - 1, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y - 1, e.first.z + 1)) != blocks.end());
			topRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y + 1, e.first.z + 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y + 1, e.first.z + 1)) != blocks.end());
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Front, occlusionFactor);
		}
		if (blocks.find(ivec3(e.first.x, e.first.y, e.first.z - 1)) == blocks.end())
		{
			topLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y + 1, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y + 1, e.first.z - 1)) != blocks.end());
			bottomLeftFactor = findAOFactor(
				blocks.find(ivec3(e.first.x + 1, e.first.y, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y - 1, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x + 1, e.first.y - 1, e.first.z - 1)) != blocks.end());
			bottomRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y - 1, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y - 1, e.first.z - 1)) != blocks.end());
			topRightFactor = findAOFactor(
				blocks.find(ivec3(e.first.x - 1, e.first.y, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x, e.first.y + 1, e.first.z - 1)) != blocks.end(),
				blocks.find(ivec3(e.first.x - 1, e.first.y + 1, e.first.z - 1)) != blocks.end());
			occlusionFactor = vec4(topLeftFactor, bottomLeftFactor, bottomRightFactor, topRightFactor);
			addFace(e.first, blockHalfSize, CubeFace::Back, occlusionFactor);
		}
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

	hasloaded = true;
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
	if (isempty || !hasloaded) return;

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