#pragma once

#include "Includes.h"
#include "Object.h"
#include "Block.h"
#include <unordered_map>

class Chunk : public Object
{

private:

	struct Vertex
	{
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texcoord) :
			Position(position), Normal(normal), TexCoords(texcoord) { }

		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	enum class CubeFace { Top, Bottom, Left, Right, Front, Back };

	struct KeyHash
	{
		size_t operator()(const glm::ivec3& key) const
		{
			return std::hash<int>()(key.x) ^ std::hash<int>()(key.y) ^ std::hash<int>()(key.z);
		}
	};

	std::unordered_map<glm::ivec3, Block, KeyHash> blocks;

	std::vector<Vertex> vertices;
	GLuint vbo;
	GLuint vao;

	void addFace(glm::vec3 pos, float size, CubeFace face);

public:

	Chunk();
	~Chunk();

	void load(int size);
	void draw();

};