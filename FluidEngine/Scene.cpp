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

	cameraTarget = new CameraTarget();
	cameraTarget->setActive(true);
	camera = new CameraFPS();
	camera->setPosition(vec3(0, 32, 0));
	
	light = new Light();
	light->setPosition(vec3(60, 80, 60));
	lightBox = new Object(light->getPosition());
	lightBox->getMaterial()->setEmissiveColor(vec3(0.8, 0, 0));

	skyQuad = new Quad();
	spheres = new Spheres();
	testObj = new Object(vec3(0, 0, 0));
	testObj->setScale(vec3(0.2f));
	testObj->getMaterial()->setEmissiveColor(vec3(1.0));
	
	plane = new Airplane(vec3(0, 32, 0));
	plane->setBoundingBoxVisible(true);
	cameraTarget->setDistance(8.0f);
	cameraTarget->setTargetObject(plane);
	cameraTarget->setOrientation(180.0f, -15.0f);

	chunkManager = new ChunkManager();
	//chunk->getMaterial()->setEmissiveColor(vec3(1.0));
	
	collision = new Collision();
}

Scene::~Scene()
{

}

void Scene::load()
{
	int loadst = glutGet(GLUT_ELAPSED_TIME);

	skyQuad->load("lake");
	// gen buffers gives wrong id??
	//thread tChunkMgr(&ChunkManager::load, chunkManager, "hm.jpg", vec3(4, 4, 1), vec3(16, 16, 32), 1.0f);
	//tChunkMgr.join();
	//chunkManager->load("hm.jpg", vec3(4, 4, 1), vec3(8, 8, 32), 1.0f);
	chunkManager->load("hm.jpg", vec3(8, 8, 1), vec3(32, 32, 32), 1.0f);
	spheres->load();
	testObj->load("cube.obj");
	lightBox->load("cube.obj");
	plane->load();
	
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
		chunkManager->update(plane->getPosition());
		plane->update(dt);

		collision->resolveTerrainObject(chunkManager, plane);

		//camera->update(dt);
		cameraTarget->update(dt);

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
	lightBox->draw();
	plane->draw();
	
	glutSwapBuffers();
}

void Scene::reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	camera->setResolution(width, height);
	cameraTarget->setResolution(width, height);
	window_width = width;
	window_height = height;
}

// Mouse buttons
void Scene::mouse(int button, int state)
{
	camera->mouse(button, state);
	cameraTarget->mouse(button, state);
}

// Mouse moved with button press
void Scene::mouseMove(int x, int y)
{
	camera->mouseMotion(x, y);
	cameraTarget->mouseMotion(x, y);
}

// Mouse moved without button press
void Scene::mouseMovePassive(int x, int y)
{
	camera->mouseMotionPassive(x, y);
	cameraTarget->mouseMotionPassive(x, y);
}

// Mouse wheel
void Scene::mouseWheel(int button, int dir, int x, int y)
{
	camera->mouseWheel(dir);
	cameraTarget->mouseWheel(dir);
}

// Common ascii keys
void Scene::keyboard(unsigned char key)
{
	if (key == 27) exit(0);
	camera->keyboard(key);
	plane->keyboard(key);
}

// Common keys up event
void Scene::keyboardUp(unsigned char key)
{
	camera->keyboardUp(key);
	plane->keyboardUp(key);
}

// Function, arrow and other special keys
void Scene::keyboardSpecial(int key)
{
	camera->keyboardSpecial(key);
	plane->keyboardSpecial(key);
}

// Special key up event
void Scene::keyboardSpecialUp(int key)
{
	camera->keyboardSpecialUp(key);
	plane->keyboardSpecialUp(key);
}