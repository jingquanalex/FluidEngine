#pragma once

#include "globalIncludes.h"
#include <string>
#include "Camera.h"

class Shader
{

	static bool hasUboMatrices;

private:

	GLuint program;
	GLuint uboMatrices;

	std::string readFile(std::string filename);
	GLuint makeProgram(std::string name);

public:

	Shader(std::string shadername);
	~Shader();

	void update(float dt, Camera* camera);

	GLuint getProgram() const;

};