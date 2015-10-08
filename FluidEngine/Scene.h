#pragma once

#include "Timer.h"
#include "Camera.h"
#include "Object.h"

class Scene
{

private:

	Camera* camera;

	Object* testObj;

public:

	Scene(Camera* camera);
	~Scene();

	// === Functions ===

	void load();
	void update(float dt);
	void draw();


	// === Callbacks ===

	void mouse(int button, int state);
	void keyboard(unsigned char key);
	void keyboardUp(unsigned char key);
	void keyboardSpecial(int key);
	void keyboardSpecialUp(int key);

};