#pragma once

#include "globalIncludes.h"
#include <string>

class Mesh
{

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Texture 
	{
		GLuint id;
		std::string type;
	};

};