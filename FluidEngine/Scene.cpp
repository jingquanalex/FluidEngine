#include "Scene.h"

using namespace glm;
using namespace std;

Scene::Scene(Camera* camera)
{
	this->camera = camera;

	testObj = new Object();
	testObj2 = new Object(vec3(1, 1, 0));
	testObj2->setRotation(vec3(0, 45, 0));
	spheres = new Spheres();
}

Scene::~Scene()
{

}

void Scene::load()
{
	testObj->load("!cube", "basic");
	testObj2->load("!cube", "basic");
	spheres->load();
}

void Scene::update(float dt)
{
	testObj->update(dt, camera);
	testObj2->update(dt, camera);
	spheres->update(dt, camera);
}

void Scene::draw()
{
	//testObj->draw();
	//testObj2->draw();
	spheres->draw();
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