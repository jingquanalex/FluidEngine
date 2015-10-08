#include "Scene.h"

using namespace glm;
using namespace std;

extern int window_width;
extern int window_height;

Scene::Scene(Camera* camera)
{
	this->camera = camera;

	this->testObj = new Object(camera);
}

Scene::~Scene()
{

}

void Scene::load()
{
	testObj->load();
}

void Scene::update(float dt)
{
	testObj->update(dt);
}

void Scene::draw()
{
	testObj->draw();
}

void Scene::mouse(int button, int state)
{
	
}

void Scene::keyboard(unsigned char key)
{
	
}

void Scene::keyboardUp(unsigned char key)
{
	
}

void Scene::keyboardSpecial(int key)
{
	
}

void Scene::keyboardSpecialUp(int key)
{
	
}