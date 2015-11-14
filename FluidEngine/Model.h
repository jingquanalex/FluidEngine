#pragma once

#include "Includes.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL2.h>
#include "Mesh.h"

class Model
{

private:

	Mesh* cube;
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string name);
	GLint TextureFromFile(const char* path);

public:

	Model(std::string filename);
	~Model();

	void draw(GLuint program);

};