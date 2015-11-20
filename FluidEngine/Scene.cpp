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
	glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader::setupUniformBuffers();

	frameTimer = new Timer(1.0f);
	frameTimer->start();

	//camera = new CameraTarget(vec3(0), 5.0f);
	camera = new CameraFPS();
	camera->setPosition(vec3(0, 33, 5));

	light = new Light();
	light->setPosition(vec3(0, 50, 0));

	skyQuad = new Quad();
	spheres = new Spheres();
	testObj = new Object(vec3(0, 0, 0));
	testObj->getMaterial()->setEmissiveColor(vec3(1.0));
	testObj2 = new Object(vec3(0, 2, 0));
	testObj2->setRotation(vec3(0, 45, 0));
	plane = new Object();

	chunkManager = new ChunkManager();
	//chunk->getMaterial()->setEmissiveColor(vec3(1.0));
	
	
}

Scene::~Scene()
{

}

void Scene::load()
{
	int loadst = glutGet(GLUT_ELAPSED_TIME);

	skyQuad->load("lake");
	// gen buffers gives wrong id??
	//thread tChunkMgr(&ChunkManager::load, chunkManager, "hm.jpg", vec3(2, 0, 1), 32, 1.0f);
	//tChunkMgr.join();
	chunkManager->load("hm.jpg", vec3(8, 8, 4), vec3(8, 8, 8), 1.0f);
	spheres->load();
	testObj->load("cube.obj");
	testObj2->load("cube.obj");
	plane->load("plane.obj");
	
	float loadtime = (glutGet(GLUT_ELAPSED_TIME) - loadst) / 1000.0f;
	cout << "Load time: " << loadtime << "s" << endl;
}

void Scene::idle()
{
	// frameTime - time to render a frame in seconds
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	float frameTime = (currentTime - previousTime) / 1000.0f;
	if (frameTime > maxframeTime) frameTime = maxframeTime;
	previousTime = currentTime;

	// Update logic at a constant dt, seperate from frame time
	accumulator += frameTime;
	while (accumulator >= dt)
	{
		chunkManager->update(camera->getPosition());
		//light->setPosition(camera->getPosition());
		camera->update(dt);

		accumulator -= dt;
	}

	// Calculate frames per second
	fps++;
	if (frameTimer->ticked())
	{
		stringstream title;
		title << g_windowTitle << " [" << fps << "]";
		glutSetWindowTitle(title.str().c_str());
		fps = 0;
	}

	// Update all timers in timer list
	Timer::updateTimers(frameTime);

	glutPostRedisplay();
}

void Scene::display()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyQuad->draw();
	chunkManager->draw(skyQuad->getCubeMapID());
	//spheres->draw();
	testObj->draw();
	//testObj2->draw();
	//plane->draw();
	
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

// Mouse wheel
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