#pragma once

#include "Timer.h"
#include "Camera.h"
#include "Object.h"
#include "Spheres.h"
#include "Light.h"

class Scene
{

private:

	Camera* camera;
	Light* light;

	Object* testObj;
	Object* testObj2;
	Spheres* spheres;
	Object* plane;

	

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