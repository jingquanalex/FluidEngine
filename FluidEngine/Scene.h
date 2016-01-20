#pragma once

#include <thread>
#include "Timer.h"
#include "CameraFPS.h"
#include "CameraTarget.h"
#include "Particles.h"
#include "Light.h"
#include "Quad.h"
#include "ChunkManager.h"

class Scene
{

private:

	Timer* frameTimer;
	CameraFPS* camera;

	Quad* skyQuad;
	Particles* particles;

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