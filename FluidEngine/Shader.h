#pragma once

#include "Includes.h"
#include <string>

class Shader
{

private:

	GLuint program;

	std::string readFile(std::string filename);
	GLuint makeProgram(std::string name);

public:

	static GLuint uboMatrices;
	static GLuint uboLighting;

	Shader(std::string shadername);
	~Shader();

	void update(float dt);

	GLuint getProgram() const;

};