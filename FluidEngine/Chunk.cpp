#include "Chunk.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Chunk::Chunk() : Object()
{

}

Chunk::~Chunk()
{
	
}

// Build the 3D array of blocks of size in all dimension
void Chunk::load(int size)
{
	//glPolygonMode(GL_FRONT, GL_LINE);

	// Generate chunk based on heightmap
	int mapWidth, mapHeight, mapChannels;
	unsigned char *heightMap = SOIL_load_image((g_mediaDirectory + "hm.jpg").c_str(), 
		&mapWidth, &mapHeight, &mapChannels, SOIL_LOAD_L);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			// Find pixel color value at index X and Y
			int cellWidth = mapWidth / size;
			int indexX = cellWidth / 2 + cellWidth * i;
			int indexY = cellWidth / 2 + cellWidth * j;
			unsigned char value = heightMap[indexX + indexY * mapWidth];

			// Generate block if sampleZ lower than color value
			for (int k = 0; k < size; k++)
			{
				int sampleValue = 256 / size / 2 + 256 / size * k;
				if (sampleValue < value)
				{
					blocks.insert(make_pair(ivec3(i, k, j), Block()));
				}
			}
		}
	}

	SOIL_free_image_data(heightMap);

	// Generate a chunk of blocks
	/*for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < size; k++)
			{
				blocks.insert(make_pair(ivec3(i, j, k), Block()));
			}
		}
	}*/

	// Create mesh for each block
	for (pair<vec3, Block> e : blocks)
	{
		// Make face only if no neighbor voxel present
		if (!blocks.count(ivec3(e.first.x + 1, e.first.y, e.first.z)))
			addFace(e.first, 0.5f, CubeFace::Right);
		if (!blocks.count(ivec3(e.first.x - 1, e.first.y, e.first.z)))
			addFace(e.first, 0.5f, CubeFace::Left);
		if (!blocks.count(ivec3(e.first.x, e.first.y + 1, e.first.z)))
			addFace(e.first, 0.5f, CubeFace::Top);
		if (!blocks.count(ivec3(e.first.x, e.first.y - 1, e.first.z)))
			addFace(e.first, 0.5f, CubeFace::Bottom);
		if (!blocks.count(ivec3(e.first.x, e.first.y, e.first.z + 1)))
			addFace(e.first, 0.5f, CubeFace::Front);
		if (!blocks.count(ivec3(e.first.x, e.first.y, e.first.z - 1)))
			addFace(e.first, 0.5f, CubeFace::Back);
	}

	// Make buffers for mesh
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
void Chunk::draw()
{
	GLuint program = shader->getProgram();
	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "material.emissive"), 1, value_ptr(material.getEmissiveColor()));
	glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, value_ptr(material.getAmbientColor()));
	glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, value_ptr(material.getDiffuseColor()));
	glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, value_ptr(material.getSpecularColor()));
	glUniform1f(glGetUniformLocation(program, "material.shininess"), material.getShininess());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, defaultTexId);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}