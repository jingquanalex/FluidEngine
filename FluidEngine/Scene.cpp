#include "Scene.h"

using namespace glm;
using namespace std;

Scene::Scene(Camera* camera)
{
	this->camera = camera;

	testObj = new Object(camera);
	testObj2 = new Object(camera);
	testObj2->setPosition(vec3(1, 1, 0));
}

Scene::~Scene()
{

}

void Scene::load()
{
	testObj->load();
	testObj2->load();
}

void Scene::update(float dt)
{
	testObj->update(dt);
	testObj2->update(dt);
}

void Scene::draw()
{
	testObj->draw();
	testObj2->draw();
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