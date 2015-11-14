#pragma once

#include "Includes.h"
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

class Shader
{

private:

	static std::vector<std::pair<GLuint, std::string>> programList;

	GLuint program;

	std::string readFile(std::string filename);
	GLuint makeProgram(std::string name);

public:

	static GLuint uboMatrices;
	static GLuint uboLighting;
	static void setupUniformBuffers();

	Shader(std::string shadername);
	~Shader();

	void update(float dt);

	GLuint getProgram() const;

};