#pragma once

#include "Timer.h"
#include "Camera.h"
#include "CameraFPS.h"
#include "CameraTarget.h"
#include "Object.h"
#include "Spheres.h"
#include "Light.h"
#include "Quad.h"
#include "ChunkManager.h"
#include "Airplane.h"
#include <thread>

class Scene
{

private:

	Timer* frameTimer;
	Camera* camera;
	CameraTarget* cameraTarget;
	Light* light;

	Quad* skyQuad;
	Object* testObj;
	Object* lightBox;
	Spheres* spheres;
	Airplane* plane;
	ChunkManager* chunkManager;

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