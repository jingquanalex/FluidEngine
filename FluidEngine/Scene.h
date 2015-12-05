#pragma once

#include <thread>
#include "Timer.h"
#include "CameraFPS.h"
#include "CameraTarget.h"
#include "Spheres.h"
#include "Light.h"
#include "Quad.h"
#include "ChunkManager.h"
#include "Airplane.h"
#include "Collision.h"

class Scene
{

private:

	Timer* frameTimer;
	CameraTarget* cameraTarget;
	CameraFPS* cameraLight;
	Light* light;

	Quad* skyQuad;
	Quad* screenQuad;
	Object* groundObj;
	Object* lightBox;
	Spheres* spheres;
	Airplane* plane;
	ChunkManager* chunkManager;
	Collision* collision;

	const float maxframeTime = 1 / 4.0f;
	const float dt = 1 / 60.0f;

	float accumulator = 0.0f;
	int fps = 0;
	int currentTime, previousTime = 0;

public:

	Scene();
	~Scene();

	// === Functions ===

	void load();
	void drawScene();

	// === Callbacks ===

	void idle();
	void display();
	void reshape(int width, int height);
	void mouse(int button, int state);
	void mouseMove(int x, int y);
	void mouseMovePassive(int x, int y);
	void mouseWheel(int button, int dir, int x, int y);
	void keyboard(unsigned char key);
	void keyboardUp(unsigned char key);
	void keyboardSpecial(int key);
	void keyboardSpecialUp(int key);

};