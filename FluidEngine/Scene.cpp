#include "Scene.h"

using namespace glm;
using namespace std;

extern string g_windowTitle;
extern int window_width;
extern int window_height;

Scene::Scene()
{
	srand(static_cast<unsigned> (time(0)));
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader::setupUniformBuffers();

	frameTimer = new Timer(1.0f);
	frameTimer->start();
	//camera = new CameraTarget(vec3(0.0f, 0.0f, 0.0f), 5.0f);
	camera = new CameraFPS();
	light = new Light();

	testObj = new Object(vec3(2, 2, 0));
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
	spheres->load();
	testObj->load("cube.obj");
	testObj2->load("cube.obj");
	plane->load("plane.obj");
}

void Scene::idle()
{
	// Calculate frame time in seconds
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	float frameTime = (currentTime - previousTime) / 1000.0f;
	if (frameTime > maxframeTime) frameTime = maxframeTime;
	previousTime = currentTime;

	// Update at a constant dt
	accumulator += frameTime;
	while (accumulator >= dt)
	{
		camera->update(dt);
		testObj->update(dt);
		testObj2->update(dt);
		//spheres->update(dt);
		plane->update(dt);

		light->setPosition(camera->getPosition());

		accumulator -= dt;
	}

	// Calculate FPS
	fps++;
	if (frameTimer->ticked())
	{
		stringstream title;
		title << g_windowTitle << " [" << fps << "]";
		glutSetWindowTitle(title.str().c_str());
		fps = 0;
	}

	// Update timers in timer list
	Timer::updateTimers(frameTime);

	glutPostRedisplay();
}

void Scene::display()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	testObj->draw();
	testObj2->draw();
	//spheres->draw();
	plane->draw();

	glutSwapBuffers();
}
	

void Scene::reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	camera->setResolution(width, height);
	window_width = width;
	window_height = height;
}

// Mouse buttons
void Scene::mouse(int button, int state)
{
	camera->mouse(button, state);
}

// Mouse moved with button press
void Scene::mouseMove(int x, int y)
{
	camera->mouseMotion(x, y);
}

// Mouse moved without button press
void Scene::mouseMovePassive(int x, int y)
{
	camera->mouseMotionPassive(x, y);
}


void Scene::mouseWheel(int button, int dir, int x, int y)
{
	camera->mouseWheel(dir);
}

// Common ascii keys
void Scene::keyboard(unsigned char key)
{
	if (key == 27) exit(0);
	camera->keyboard(key);
}

// Common keys up event
void Scene::keyboardUp(unsigned char key)
{
	camera->keyboardUp(key);
}

// Function, arrow and other special keys
void Scene::keyboardSpecial(int key)
{
	camera->keyboardSpecial(key);
}

// Special key up event
void Scene::keyboardSpecialUp(int key)
{
	camera->keyboardSpecialUp(key);
}