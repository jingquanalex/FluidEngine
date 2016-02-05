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

	camera = new CameraFPS();
	camera->setActive(true);

	skyQuad = new Quad();

	particles = new Particles();
	
}

Scene::~Scene()
{

}

void Scene::load()
{
	int loadStart = glutGet(GLUT_ELAPSED_TIME);

	skyQuad->load("lake", "skyquad");
	particles->load(camera);
	
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
	dt = 1 / 60.0f;
	while (accumulator >= dt)
	{
		particles->update(dt);

		accumulator -= dt;
	}

	// Temp
	camera->update(frameTime);

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
	// Render scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	skyQuad->draw();
	particles->draw();
	
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
	particles->mouse(button, state);
}

// Mouse moved with button press
void Scene::mouseMove(int x, int y)
{
	camera->mouseMotion(x, y);
	particles->mouseMove(x, y);
}

// Mouse moved without button press
void Scene::mouseMovePassive(int x, int y)
{
	camera->mouseMotionPassive(x, y);
	particles->mouseMovePassive(x, y);
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
	particles->keyboard(key);
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