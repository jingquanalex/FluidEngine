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
	
	light = new Light();
	light->setPosition(vec3(128, 120, 140));
	
	cameraLight = new CameraFPS();
	cameraLight->setOrtho(true);
	cameraLight->setFov(55.0f);
	cameraLight->setPosition(light->getPosition());
	cameraLight->setTargetPoint(vec3(64, 0, 64));

	skyQuad = new Quad();
	screenQuad = new Quad();
	spheres = new Spheres();
	groundObj = new Object(vec3(0, 20, 0));
	groundObj->getMaterial()->setEmissiveColor(vec3(1.0));
	lightBox = new Object(light->getPosition());
	lightBox->getMaterial()->setEmissiveColor(vec3(0.8, 0, 0));
	
	plane = new Airplane(vec3(64, 5.3, 6));
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
	int loadStart = glutGet(GLUT_ELAPSED_TIME);

	light->load();
	skyQuad->load("lake", "skyquad");
	screenQuad->load();
	// gen buffers gives wrong id??
	//thread tChunkMgr(&ChunkManager::load, chunkManager, "hm.jpg", vec3(4, 4, 1), vec3(16, 16, 32), 1.0f);
	//tChunkMgr.join();
	//chunkManager->load("hm.jpg", vec3(4, 4, 4), vec3(8, 8, 8), 1.0f);
	//chunkManager->load("hm.jpg", vec3(1, 1, 1), vec3(32, 32, 32), 1.0f);
	chunkManager->load("hm.jpg", vec3(1, 1, 1), vec3(128, 128, 32), 1.0f);
	spheres->load();
	groundObj->load("cube.obj");
	lightBox->load("cube.obj");
	plane->load();
	
	float loadtime = (glutGet(GLUT_ELAPSED_TIME) - loadStart) / 1000.0f;
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
		plane->update(dt);
		chunkManager->update(plane->getPosition());
		
		collision->resolveTerrainPlane(chunkManager, plane);

		cameraLight->update(dt);
		cameraTarget->update(dt);

		light->update(cameraLight);

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
	// Render to shadowmap buffer
	glBindFramebuffer(GL_FRAMEBUFFER, light->getDepthFbo());
	glViewport(0, 0, light->getDepthMapSize().x, light->getDepthMapSize().y);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_FRONT);
	chunkManager->draw(light);
	plane->draw(light, 1);
	glCullFace(GL_BACK);
	

	// Render scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	drawScene();
	
	glutSwapBuffers();
}

void Scene::drawScene()
{
	//screenQuad->draw(light, cameraLight);
	skyQuad->draw();
	chunkManager->draw(skyQuad->getCubeMap(), light);
	plane->draw(light);
	//spheres->draw();
	groundObj->draw();
	lightBox->draw();
}

void Scene::reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	cameraLight->setResolution(width, height);
	cameraTarget->setResolution(width, height);
	window_width = width;
	window_height = height;
}

// Mouse buttons
void Scene::mouse(int button, int state)
{
	cameraLight->mouse(button, state);
	cameraTarget->mouse(button, state);
}

// Mouse moved with button press
void Scene::mouseMove(int x, int y)
{
	cameraLight->mouseMotion(x, y);
	cameraTarget->mouseMotion(x, y);
}

// Mouse moved without button press
void Scene::mouseMovePassive(int x, int y)
{
	cameraLight->mouseMotionPassive(x, y);
	cameraTarget->mouseMotionPassive(x, y);
}

// Mouse wheel
void Scene::mouseWheel(int button, int dir, int x, int y)
{
	cameraLight->mouseWheel(dir);
	cameraTarget->mouseWheel(dir);
}

// Common ascii keys
void Scene::keyboard(unsigned char key)
{
	// For cameraTarget views
	switch (key)
	{
	case '5':
		cameraTarget->setTargetObject(plane, groundObj);
		break;
	case '6':
		cameraTarget->setTargetObject(plane, lightBox);
		break;
	case 'c':
		plane->setBoundingBoxVisible(!plane->getBoundingBoxVisible());
		break;
	case 'v':
		if (cameraLight->getActive())
		{
			cameraLight->setActive(false);
			cameraTarget->setActive(true);
		}
		else
		{
			cameraLight->setActive(true);
			cameraTarget->setActive(false);
		}
		break;
	}

	if (key == 27) exit(0);
	cameraLight->keyboard(key);
	cameraTarget->keyboard(key);
	if (!cameraLight->getActive())
	{
		plane->keyboard(key);
	}
}

// Common keys up event
void Scene::keyboardUp(unsigned char key)
{
	cameraLight->keyboardUp(key);
	if (!cameraLight->getActive())
	{
		plane->keyboardUp(key);
	}
}

// Function, arrow and other special keys
void Scene::keyboardSpecial(int key)
{
	cameraLight->keyboardSpecial(key);
	if (!cameraLight->getActive())
	{
		plane->keyboardSpecial(key);
	}
}

// Special key up event
void Scene::keyboardSpecialUp(int key)
{
	cameraLight->keyboardSpecialUp(key);
	if (!cameraLight->getActive())
	{
		plane->keyboardSpecialUp(key);
	}
}