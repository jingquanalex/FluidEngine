#pragma once

#include "globalIncludes.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL2.h>
#include "Mesh.h"

// Mesh loading code from tutorial: http://learnopengl.com/#!Model-Loading/Model

class Model
{

	// Predefined unit primitives data
	static const GLfloat triVertices[9];
	static const GLuint triIndices[3];
	static const GLfloat cubeVertices[24];
	static const GLfloat cubeTexCoords[24];
	static const GLuint cubeIndices[36];

private:

	Mesh* cube;
	std::vector<Mesh> meshes;
	std::string filename;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string name);
	GLint TextureFromFile(const char* path, std::string directory);

public:

	Model(std::string filename);
	~Model();

	void draw(GLuint program);

};