#pragma once

#include "Includes.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL2.h>
#include "Mesh.h"

struct BoundingBox
{
	BoundingBox() { }
	BoundingBox(glm::vec3 min, glm::vec3 max) : Min(min), Max(max) { }
	glm::vec3 Min, Max;
	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec3> SATNormals;
};

class Model
{

private:

	BoundingBox bbox;
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

	BoundingBox getBoundingBox() const;

};