#include "Model.h"

using namespace glm;
using namespace std;
using namespace Assimp;

extern string g_mediaDirectory;

Model::Model(string filename)
{
	this->filename = filename;
	if (filename == "!cube")
	{
		// Make cube data vectors
		vector<Vertex> vertices;
		for (int i = 0; i < 8; i++)
		{
			Vertex vertex;
			vertex.Position = vec3(cubeVertices[i * 3 + 0], cubeVertices[i * 3 + 1], cubeVertices[i * 3 + 2]);
			vertex.Normal = vec3();
			vertex.TexCoords = vec2(cubeTexCoords[i * 2 + 0], cubeTexCoords[i * 2 + 1]);
			vertices.push_back(vertex);
		}
		vector<GLuint> indices(cubeIndices, cubeIndices + 36);
		vector<Texture> textures;
		Texture texture;
		texture.id = TextureFromFile("cat.png", g_mediaDirectory);
		textures.push_back(texture);

		cube = new Mesh(vertices, indices, textures);
	}
	else
	{
		loadModel(g_mediaDirectory + "model/" + filename);
	}
}

Model::~Model()
{

}

void Model::draw(GLuint program)
{
	if (filename == "!cube")
	{
		cube->bindTexture(program);
		cube->draw();
	}
	else
	{
		for (Mesh& mesh : meshes)
		{
			mesh.bindTexture(program);
			mesh.draw();
		}
	}
}

void Model::loadModel(string path)
{
	Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any)
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// Then do the same for each of its children
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

// Extract revelent data from assimp mesh for our dataset
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	// Process vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.Position = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0])
		{
			vertex.TexCoords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.TexCoords = vec2();
		}

		vertices.push_back(vertex);
	}
	
	// Process indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = loadMaterialTextures(material, 
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		vector<Texture> specularMaps = loadMaterialTextures(material, 
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	vector<Texture> textures_loaded;

	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		GLboolean skip = false;

		for (GLuint j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j].path == str.C_Str())
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			// If texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // Add to loaded textures
		}
	}

	return textures;
}

GLint Model::TextureFromFile(const char* path, string directory)
{
	// Generate texture ID and load texture data 
	string filename = string(path);
	filename = directory + '/' + filename;

	GLuint textureID = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, 
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

const GLfloat Model::triVertices[] =
{
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};

const GLuint Model::triIndices[] =
{
	0, 1, 2
};

// Cube vertices, color, texcoord
const GLfloat Model::cubeVertices[] =
{
	// Front
	-0.5, -0.5, 0.5,
	0.5, -0.5, 0.5,
	0.5, 0.5, 0.5,
	-0.5, 0.5, 0.5,

	// Back
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5, 0.5, -0.5,
	-0.5, 0.5, -0.5
};

const GLfloat Model::cubeTexCoords[] =
{
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,

	1.0, 0.0,
	0.0, 0.0,
	0.0, 1.0,
	1.0, 1.0
};

const GLuint Model::cubeIndices[] =
{
	// Front
	0, 1, 2,
	2, 3, 0,
	// Top
	3, 2, 6,
	6, 7, 3,
	// Back
	7, 6, 5,
	5, 4, 7,
	// Bottom
	4, 5, 1,
	1, 0, 4,
	// Left
	4, 0, 3,
	3, 7, 4,
	// Right
	1, 5, 6,
	6, 2, 1
};