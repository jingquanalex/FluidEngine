#include "Scene.h"

using namespace glm;
using namespace std;

Scene::Scene(Camera* camera)
{
	this->camera = camera;

	light = new Light();
	testObj = new Object();
	testObj2 = new Object(vec3(1, 1, 0));
	testObj2->setRotation(vec3(0, 45, 0));
	spheres = new Spheres();
	plane = new Object();

	
}

Scene::~Scene()
{

}

void Scene::load()
{
	testObj->load("cube.obj", "basic");
	testObj2->load("!cube");
	spheres->load();
	plane->load("plane.obj");
}

void Scene::update(float dt)
{
	testObj->update(dt);
	testObj2->update(dt);
	//spheres->update(dt);
	plane->update(dt);
}

void Scene::draw()
{
	testObj->draw();
	testObj2->draw();
	//spheres->draw();
	plane->draw();
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