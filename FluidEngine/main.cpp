#include "main.h"

using namespace std;
using namespace glm;

void init()
{
	camera = new CameraTarget(vec3(0.0f, 0.0f, 0.0f), 5.0f);
	fpsTimer = new Timer(1.0f);
	fpsTimer->start();
	scene = new Scene(camera);
	scene->load();

	glEnable(GL_DEPTH_TEST);
}

// Update loop for game logic and physics.
void idle()
{
	// Calculate frame time
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	float frameTime = (currentTime - previousTime) / 1000.0f;
	if (frameTime > maxframeTime) frameTime = maxframeTime;
	previousTime = currentTime;

	// Calculate FPS
	fps++;
	if (fpsTimer->hasTicked())
	{
		stringstream title;
		title << g_windowTitle << " [" << fps << "]";
		glutSetWindowTitle(title.str().c_str());

		fps = 0;
	}

	// Update timers in timer list
	Timer::updateTimers(frameTime);

	// Update logic once every dt
	accumulator += frameTime;
	while (accumulator >= dt)
	{
		accumulator -= dt;

		camera->update(dt);
		scene->update(dt);
	}
	
	glutPostRedisplay();
}

// Update loop for rendering.
void display()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene->draw();
	
	glutSwapBuffers();
}

// Window dimension changed, recalculate camera proj matrix.
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	camera->setResolution(width, height);
	window_width = width;
	window_height = height;
}

// Mouse buttons
void mouse(int button, int state, int x, int y)
{
	camera->mouse(button, state);
	//scene->mouse(button, state);
}

// Mouse moved with button press
void mouseMove(int x, int y)
{
	camera->mouseMotion(x, y);
}

// Mouse moved without button press
void mouseMovePassive(int x, int y)
{
	camera->mouseMotionPassive(x, y);
}

void mouseWheel(int button, int dir, int x, int y)
{
	camera->mouseWheel(dir);
}

// Common ascii keys.
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) exit(0);
	//else if (key == 'l') scene->load();
	//scene->keyboard(key);
	camera->keyboard(key);
}

// Common keys up event.
void keyboardUp(unsigned char key, int x, int y)
{
	//scene->keyboardUp(key);
	camera->keyboardUp(key);
}

// Function, arrow and other special keys.
void keyboardSpecial(int key, int x, int y)
{
	camera->keyboardSpecial(key);
	//scene->keyboardSpecial(key);
}

// Special key up event.
void keyboardSpecialUp(int key, int x, int y)
{
	camera->keyboardSpecialUp(key);
	//scene->keyboardSpecialUp(key);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(-1, -1);
	glutInitWindowSize(window_width, window_height);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(g_windowTitle.c_str());

	// Enable fullscreen mode if arg -fullscreen specified
	stringstream ss;
	if (argc == 2) ss << argv[1] << "";
	if (ss.str() == "-fullscreen")
	{
		glutGameModeString("1280x800:32@60");
		glutEnterGameMode();
	}

	glewInit();
	printf("OpenGL version: %s \nGLEW version: %s \n", 
		glGetString(GL_VERSION), glewGetString(GLEW_VERSION));

	// This will pick the best possible CUDA capable device
	/*cudaDeviceProp deviceProp;
	int devID = gpuGetMaxGflopsDeviceId();
	checkCudaErrors(cudaSetDevice(devID));
	checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devID));
	printf("GPU Device %d: \"%s\" with compute capability %d.%d\n", 
		devID, deviceProp.name, deviceProp.major, deviceProp.minor);
		*/

	init();

	// Callback Functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutMouseWheelFunc(mouseWheel);
	glutPassiveMotionFunc(mouseMovePassive);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(keyboardSpecial);
	glutSpecialUpFunc(keyboardSpecialUp);

	glutMainLoop();

	return 0;
}