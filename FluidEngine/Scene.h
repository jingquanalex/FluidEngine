#pragma once

#include <thread>
#include <stdio.h>
#include <iomanip>
#include "Timer.h"
#include "CameraFPS.h"
#include "CameraTarget.h"
#include "Particles.h"
#include "Light.h"
#include "Quad.h"
#include "ChunkManager.h"
#include "Font.h"

class Scene
{

private:

	Timer* frameTimer;
	Timer* frameAvgTimer;
	Timer* particlesTimer;
	//CameraFPS* camera;
	CameraTarget* camera;
	Font* font;
	Object* box;
	Particles* particles;

	float fpsUpdateInterval = 0.5f;
	float dt = 1 / 60.0f;

	float fps, fpsAvg;
	std::vector<float> listFps;
	int currentTime, previousTime = 0;
	int timeBase = 0, frame_accum = 0;
	int displayTextMode = 1;

	std::string Trim(float d);

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