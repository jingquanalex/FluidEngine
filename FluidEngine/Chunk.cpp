#include "Chunk.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Chunk::Chunk() : Object()
{
	matModel = mat4();
	isempty = false;
	//material.setAmbientColor(vec3(0.3f, 0.3f, 0.6f));
	//material.setDiffuseColor(vec3(1.0f));
	material.setSpecularColor(vec3(0.1f));
	material.setShininess(8.0f);
}

Chunk::~Chunk()
{
	
}

// Build the 3D array of blocks of size in all dimension
// sectionBuffer(x index, y index, section width)
void Chunk::load(unsigned char *heightMap, Section sectionBuffer, int chunksize, float blocksize)
{
	blocks.reserve(chunksize * chunksize * chunksize);

	// Generate chunk of blocks based on a section of heightmap
	for (int i = 0; i < chunksize; i++)
	{
		for (int j = 0; j < chunksize; j++)
		{
			// Sample pixel color of chunk[i, j] at section[x, y] of heightmap
			int sectionWidth = sectionBuffer.mapwidth / sectionBuffer.sections.x;
			int cells = sectionWidth / chunksize;
			int indexX = cells / 2 + cells * i + sectionBuffer.index.x * sectionWidth;
			int indexY = cells / 2 + cells * j + sectionBuffer.index.y * sectionWidth;
			unsigned char value = heightMap[indexX + indexY * sectionBuffer.mapwidth];

			// Generate block if sampleZ lower than color value
			sectionWidth = 256 / sectionBuffer.sections.z;
			cells = sectionWidth / chunksize;
			for (int k = 0; k < chunksize; k++)
			{
				int valueAtZ = cells / 2 + cells * k + sectionBuffer.index.z * sectionWidth;
				if (valueAtZ < value)
				{
					blocks.insert(make_pair(ivec3(i, k, j), Block()));
				}
			}
		}
	}

	isempty = blocks.empty();
	if (isempty) return;

	// Create mesh for each block
	blocksize /= 2;
	for (pair<vec3, Block> e : blocks)
	{
		// Make face only if no neighbor voxel present
		if (blocks.find(ivec3(e.first.x + 1, e.first.y, e.first.z)) == blocks.end())
			addFace(e.first, blocksize, CubeFace::Right);
		if (blocks.find(ivec3(e.first.x - 1, e.first.y, e.first.z)) == blocks.end())
			addFace(e.first, blocksize, CubeFace::Left);
		if (blocks.find(ivec3(e.first.x, e.first.y + 1, e.first.z)) == blocks.end())
			addFace(e.first, blocksize, CubeFace::Top);
		if (blocks.find(ivec3(e.first.x, e.first.y - 1, e.first.z)) == blocks.end())
			addFace(e.first, blocksize, CubeFace::Bottom);
		if (blocks.find(ivec3(e.first.x, e.first.y, e.first.z + 1)) == blocks.end())
			addFace(e.first, blocksize, CubeFace::Front);
		if (blocks.find(ivec3(e.first.x, e.first.y, e.first.z - 1)) == blocks.end())
			addFace(e.first, blocksize, CubeFace::Back);
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
	glBindVertexArray(0);

	verticesSize = vertices.size();
	vertices.clear();

	shader = new Shader("terrain");
}

// Make a face of a cube at center [pos] with square width of [size * 2]
// pos - cube center, size - half width (or height) of cube, face - which face to generate
void Chunk::addFace(vec3 pos, float size, CubeFace face)
{
	float offset = size * 2;

	switch (face)
	{
	case CubeFace::Front:
		vertices.push_back(Vertex(pos * offset + vec3(-size, size, size), vec3(0.0, 0.0, 1.0), vec2(0.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, -size, size), vec3(0.0, 0.0, 1.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, size, size), vec3(0.0, 0.0, 1.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, size, size), vec3(0.0, 0.0, 1.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, -size, size), vec3(0.0, 0.0, 1.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, -size, size), vec3(0.0, 0.0, 1.0), vec2(1.0, 0.0)));
		break;
	case CubeFace::Back:
		vertices.push_back(Vertex(pos * offset + vec3(size, size, -size), vec3(0.0, 0.0, -1.0), vec2(0.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, -size, -size), vec3(0.0, 0.0, -1.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, size, -size), vec3(0.0, 0.0, -1.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, size, -size), vec3(0.0, 0.0, -1.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, -size, -size), vec3(0.0, 0.0, -1.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, -size, -size), vec3(0.0, 0.0, -1.0), vec2(1.0, 0.0)));
		break;
	case CubeFace::Top:
		vertices.push_back(Vertex(pos * offset + vec3(-size, size, -size), vec3(0.0, 1.0, 0.0), vec2(0.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, size, size), vec3(0.0, 1.0, 0.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, size, -size), vec3(0.0, 1.0, 0.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, size, -size), vec3(0.0, 1.0, 0.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, size, size), vec3(0.0, 1.0, 0.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, size, size), vec3(0.0, 1.0, 0.0), vec2(1.0, 0.0)));
		break;
	case CubeFace::Bottom:
		vertices.push_back(Vertex(pos * offset + vec3(-size, -size, size), vec3(0.0, -1.0, 0.0), vec2(0.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, -size, -size), vec3(0.0, -1.0, 0.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, -size, size), vec3(0.0, -1.0, 0.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, -size, size), vec3(0.0, -1.0, 0.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, -size, -size), vec3(0.0, -1.0, 0.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, -size, -size), vec3(0.0, -1.0, 0.0), vec2(1.0, 0.0)));
		break;
	case CubeFace::Left:
		vertices.push_back(Vertex(pos * offset + vec3(-size, size, -size), vec3(-1.0, 0.0, 0.0), vec2(0.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, -size, -size), vec3(-1.0, 0.0, 0.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, size, size), vec3(-1.0, 0.0, 0.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, size, size), vec3(-1.0, 0.0, 0.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, -size, -size), vec3(-1.0, 0.0, 0.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(-size, -size, size), vec3(-1.0, 0.0, 0.0), vec2(1.0, 0.0)));
		break;
	case CubeFace::Right:
		vertices.push_back(Vertex(pos * offset + vec3(size, size, size), vec3(1.0, 0.0, 0.0), vec2(0.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, -size, size), vec3(1.0, 0.0, 0.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, size, -size), vec3(1.0, 0.0, 0.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, size, -size), vec3(1.0, 0.0, 0.0), vec2(1.0, 1.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, -size, size), vec3(1.0, 0.0, 0.0), vec2(0.0, 0.0)));
		vertices.push_back(Vertex(pos * offset + vec3(size, -size, -size), vec3(1.0, 0.0, 0.0), vec2(1.0, 0.0)));
		break;
	}
}

// Create quad for all visible faces
void Chunk::draw(GLuint envMapId)
{
	if (isempty) return;

	GLuint program = shader->getProgram();
	glUseProgram(program);
	glUniformMatrix4fv(10, 1, GL_FALSE, value_ptr(matModel));
	glUniform1i(glGetUniformLocation(program, "diffuse1"), 0);
	glUniform1i(glGetUniformLocation(program, "cubemap1"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, defaultTexID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envMapId);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, verticesSize);
	glBindVertexArray(0);
}

void Chunk::setPosition(glm::vec3 position)
{
	matModel = translate(position);
}