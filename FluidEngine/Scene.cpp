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

	Shader::setupUniformBuffers();

	frameTimer = new Timer(fpsUpdateInterval);
	frameTimer->start();
	frameAvgTimer = new Timer(9.0f);
	frameAvgTimer->start();
	sphTimer = new Timer();
	sphTimer->start();

	font = new Font();

	// Load a box model to contain the particles
	box = new Object();
	box->setScale(vec3(10));
	box->setWireframeMode(true);
	box->load("cube.obj");

	//camera = new CameraFPS();
	camera = new CameraTarget();
	camera->setActive(true);
	camera->setDistance(8.0f);
	camera->setTarget(vec3(0, -4, 0));
	camera->setOrientation(-135.0f, -35.0f);
	camera->setMouseSensitivity(0.16f);

	particles = new Particles();
}

Scene::~Scene()
{

}

void Scene::load()
{
	int loadStart = glutGet(GLUT_ELAPSED_TIME);

	// Set solver delta time
	sphTimer->setTickInterval(1 / 120.0f);
	particles->load(1 / 170.0f, camera);
	font->load("arial.ttf");
	
	float loadtime = (glutGet(GLUT_ELAPSED_TIME) - loadStart) / 1000.0f;
	cout << "Load time: " << loadtime << "s" << endl;

	previousTime = glutGet(GLUT_ELAPSED_TIME);
}

void Scene::idle()
{
	// frameTime - time to render a frame in seconds
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	float frameTime = (currentTime - previousTime) / 1000.0f;
	//if (frameTime > maxframeTime) frameTime = maxframeTime;
	previousTime = currentTime;

	/*particle_accum += frameTime;
	if (particle_accum >= 1 / 120.0f)
	{
		particles->update();
		particle_accum -= 1 / 120.0f;
	}*/

	if (sphTimer->ticked())
	{
		particles->update();
	}

	camera->update(frameTime);

	// Calculate frames per second
	frame_accum++;
	if (currentTime - timeBase > fpsUpdateInterval * 1000.0f)
	{
		fps = frame_accum * 1000.0f / (currentTime - timeBase);
		timeBase = currentTime;
		frame_accum = 0;
	}

	if (frameTimer->ticked())
	{
		if (listFps.size() > 6)  listFps.erase(listFps.begin());
		listFps.push_back(fps);
	}

	if (frameAvgTimer->ticked())
	{
		float sum = 0.0f;
		for (float n : listFps) sum += n;
		fpsAvg = sum / listFps.size();

		stringstream title;
		title << g_windowTitle << " [" << (int)fpsAvg << "]";
		glutSetWindowTitle(title.str().c_str());
	}

	// Update all timers in timer list
	Timer::updateTimers(frameTime);

	glutPostRedisplay();
}

void Scene::display()
{
	// Render particles depth to render buffer
	particles->drawDepth();

	// Render scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	box->draw();
	particles->draw();
	
	// TODO: Unoptimized
	if (displayTextMode >= 1)
	{
		font->RenderText("F1 - Toggle help display", 25.0f, (float)window_height - 35.0f, 1.0f, vec3(1));

		if (displayTextMode == 1)
		{
			stringstream ss;
			ss.setf(ios::fixed, ios_base::floatfield);
			ss << setprecision(1) << "FPS: " << fps << " (Avg: " << fpsAvg << ")";
			font->RenderText(ss.str(), 25.0f, (float)window_height - 75.0f, 1.0f, vec3(1));
			ss.str("");
			ss << "Particles: " << particles->getSolver()->getParticleCount();
			font->RenderText(ss.str(), 25.0f, (float)window_height - 95.0f, 1.0f, vec3(1));
			ss.str("");
			ss << setprecision(3) << "Rest Density: " << (particles->getSolver()->getRestDensity());
			font->RenderText(ss.str(), 25.0f, (float)window_height - 115.0f, 1.0f, vec3(1));
			ss.str("");
			ss << setprecision(10) << "Gas Constant: " << Trim(particles->getSolver()->getGasConstant());
			font->RenderText(ss.str(), 25.0f, (float)window_height - 135.0f, 1.0f, vec3(1));
			ss.str("");
			ss << setprecision(3) << "Viscosity: " << (particles->getSolver()->getViscosity());
			font->RenderText(ss.str(), 25.0f, (float)window_height - 155.0f, 1.0f, vec3(1));
			ss.str("");
			ss << setprecision(3) << "Surface Tension Force: " << (particles->getSolver()->getSurfaceTension());
			font->RenderText(ss.str(), 25.0f, (float)window_height - 175.0f, 1.0f, vec3(1));
		}
		
		if (displayTextMode >= 2)
		{
			font->RenderText("RMouse + drag - Rotate camera", 25.0f, (float)window_height - 75.0f, 1.0f, vec3(1));
			font->RenderText("RMouse + mousewheel - Move camera near/far", 25.0f, (float)window_height - 95.0f, 1.0f, vec3(1));

			font->RenderText("Particle Solver", 25.0f, (float)window_height - 135.0f, 1.0f, vec3(1));
			font->RenderText("LMouse (particle) + drag - Apply force to particle", 25.0f, (float)window_height - 155.0f, 1.0f, vec3(1));
			font->RenderText("Space - Add 100 particles", 25.0f, (float)window_height - 175.0f, 1.0f, vec3(1));
			font->RenderText("c - Clear particles", 25.0f, (float)window_height - 195.0f, 1.0f, vec3(1));
			font->RenderText("g - Toggle gravity", 25.0f, (float)window_height - 215.0f, 1.0f, vec3(1));
			font->RenderText("p - Pause simulation", 25.0f, (float)window_height - 235.0f, 1.0f, vec3(1));
			font->RenderText("q, w - Decrease/Increase rest density", 25.0f, (float)window_height - 255.0f, 1.0f, vec3(1));
			font->RenderText("e, r - Decrease/Increase gas constant", 25.0f, (float)window_height - 275.0f, 1.0f, vec3(1));
			font->RenderText("t, y - Decrease/Increase viscosity", 25.0f, (float)window_height - 295.0f, 1.0f, vec3(1));
			font->RenderText("u, i - Decrease/Increase surface tension force", 25.0f, (float)window_height - 315.0f, 1.0f, vec3(1));
			stringstream ss;
			ss.setf(ios::fixed, ios_base::floatfield);
			ss << setprecision(3) << "Rest Density: " << (particles->getSolver()->getRestDensity());
			font->RenderText(ss.str(), 25.0f, (float)window_height - 335.0f, 1.0f, vec3(1));
			ss.str("");
			ss << setprecision(10) << "Gas Constant: " << Trim(particles->getSolver()->getGasConstant());
			font->RenderText(ss.str(), 25.0f, (float)window_height - 355.0f, 1.0f, vec3(1));
			ss.str("");
			ss << setprecision(3) << "Viscosity: " << (particles->getSolver()->getViscosity());
			font->RenderText(ss.str(), 25.0f, (float)window_height - 375.0f, 1.0f, vec3(1));
			ss.str("");
			ss << setprecision(3) << "Surface Tension Force: " << (particles->getSolver()->getSurfaceTension());
			font->RenderText(ss.str(), 25.0f, (float)window_height - 395.0f, 1.0f, vec3(1));

			font->RenderText("Particle Renderer", 25.0f, (float)window_height - 435.0f, 1.0f, vec3(1));
			font->RenderText("1 - Full composition", 25.0f, (float)window_height - 455.0f, 1.0f, vec3(1));
			font->RenderText("2 - Depth", 25.0f, (float)window_height - 475.0f, 1.0f, vec3(1));
			font->RenderText("3 - Normals", 25.0f, (float)window_height - 495.0f, 1.0f, vec3(1));
			font->RenderText("4 - Diffuse", 25.0f, (float)window_height - 515.0f, 1.0f, vec3(1));
			font->RenderText("5 - Diffuse + color", 25.0f, (float)window_height - 535.0f, 1.0f, vec3(1));
			font->RenderText("6 - Thickness map + color", 25.0f, (float)window_height - 555.0f, 1.0f, vec3(1));
			font->RenderText("7 - Refraction", 25.0f, (float)window_height - 575.0f, 1.0f, vec3(1));
			font->RenderText("8 - Refraction + lighting", 25.0f, (float)window_height - 595.0f, 1.0f, vec3(1));
			font->RenderText("9 - Fresnel + reflection", 25.0f, (float)window_height - 615.0f, 1.0f, vec3(1));
			font->RenderText("0 - Points", 25.0f, (float)window_height - 635.0f, 1.0f, vec3(1));

			font->RenderText("-,= - Cycle materials", 25.0f, (float)window_height - 675.0f, 1.0f, vec3(1));
			font->RenderText("[, ] - Cycle cubemaps", 25.0f, (float)window_height - 695.0f, 1.0f, vec3(1));
			
		}
	}
	glutSwapBuffers();
}

void Scene::reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	camera->setResolution(width, height);
	window_width = width;
	window_height = height;
	font->updateProjection();
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
	if (key == 27) exit(0); // esc
	if (key == 'p')
	{
		particles->togglePauseSimulation();

		/*if (sphTimer->getIsRunning())
		{
			sphTimer->stop();
		}
		else
		{
			sphTimer->start();
		}*/
		
	}
	
	camera->keyboard(key);
	particles->keyboard(key);
}

// Common keys up event
void Scene::keyboardUp(unsigned char key)
{
	camera->keyboardUp(key);
	particles->keyboardUp(key);
}

// Function, arrow and other special keys
void Scene::keyboardSpecial(int key)
{
	if (key == GLUT_KEY_F1)
	{
		displayTextMode = ++displayTextMode > 2 ? 0 : displayTextMode;
	}

	camera->keyboardSpecial(key);
	particles->keyboardSpecial(key);
}

// Special key up event
void Scene::keyboardSpecialUp(int key)
{
	camera->keyboardSpecialUp(key);
}

string Scene::Trim(float d)
{
	size_t len = _snprintf(0, 0, "%.10f", d);
	string s(len + 1, 0);
	_snprintf(&s[0], len + 1, "%.10f", d);
	// remove nul terminator
	s.pop_back();
	// remove trailing zeros
	s.erase(s.find_last_not_of('0') + 1, string::npos);
	// add 0 if 0.
	if (s.back() == '.') s.push_back('0');
	return s;
}