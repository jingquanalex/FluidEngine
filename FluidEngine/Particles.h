#pragma once

#include "Includes.h"
#include "Object.h"
#include "WCSPH.h"
//#include "PCISPH.h"
#include "Quad.h"
#include "Font.h"

// Particle Manager: Generate, solve and render particles
// Particles are rendered on 2D quads created with geometry shader

class Particles : public Object
{

private:

	// Particle data to upload to GPU
	struct sParticle
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	std::vector<Particle> particles;
	std::vector<sParticle> sParticles;
	int count, maxCount;
	WCSPH* solver;
	//PCISPH* solver;

	GLuint vao, vbo;
	GLuint texid;

	bool statePicked;
	int mouseX, mouseY;
	int mouseLastX = 0, mouseLastY = 0;
	glm::ivec2 mouseDelta;
	Camera* camera;

	Quad* cubemaps;
	Quad* screenQuad;
	glm::ivec2 mapSize;
	int renderMode = 0;

	Shader* shaderBFilter;
	Shader* shaderNormal;
	Shader* shaderGaussian;

	GLuint sceneFbo;
	GLuint sceneMap;
	GLuint depthFbo;
	GLuint depthMap;
	GLuint colorMap;
	GLuint blurFbo;
	GLuint blurMapV;
	GLuint blurMapH;
	GLuint thickFbo;
	GLuint thickMap;
	GLuint gaussFbo;
	GLuint gaussMapV;
	GLuint gaussMapH;

	float dt;
	int heldtime = 0;
	float inctime;
	bool stateHeldtime;

	bool stateIncRestdensity = false, stateDecRestdensity = false;
	bool stateIncGasconstant = false, stateDecGasconstant = false;
	bool stateIncViscosity = false, stateDecViscosity = false;
	bool stateIncSurfacetension = false, stateDecSurfacetension = false;

public:

	Particles();
	~Particles();

	void load(float dt, Camera* camera);
	void update();
	void drawDepth();
	void draw();

	float fRandom(float low, float high);
	void addParticles(int value, float range);
	void addParticles(int numPerSide);
	void addParticles();
	void removeParticles(int value);

	void mouse(int button, int state);
	void mouseMove(int x, int y);
	void mouseMovePassive(int x, int y);
	void keyboard(unsigned char key);
	void keyboardUp(unsigned char key);
	void keyboardSpecial(int key);

	WCSPH* getSolver();

};