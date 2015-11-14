#include "main.h"

using namespace std;
using namespace glm;

void init()
{
	scene = new Scene();
	scene->load();
}

// Update loop for game logic and physics.
void idle()
{
	scene->idle();
}

// Update loop for rendering.
void display()
{
	scene->display();
}

// Window dimension changed, recalculate camera proj matrix.
void reshape(int width, int height)
{
	scene->reshape(width, height);
}


void mouse(int button, int state, int x, int y)
{
	scene->mouse(button, state);
}


void mouseMove(int x, int y)
{
	scene->mouseMove(x, y);
}

void mouseMovePassive(int x, int y)
{
	scene->mouseMovePassive(x, y);
}

void mouseWheel(int button, int dir, int x, int y)
{
	scene->mouseWheel(button, dir, x, y);
}


void keyboard(unsigned char key, int x, int y)
{
	scene->keyboard(key);
}


void keyboardUp(unsigned char key, int x, int y)
{
	scene->keyboardUp(key);
}


void keyboardSpecial(int key, int x, int y)
{
	scene->keyboardSpecial(key);
}


void keyboardSpecialUp(int key, int x, int y)
{
	scene->keyboardSpecialUp(key);
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

	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version: %s\n", glGetString(GL_VERSION));
	printf("GLEW version: %s\n", glewGetString(GLEW_VERSION));

	// This will pick the best possible CUDA capable device
	/*cudaDeviceProp deviceProp;
	int devID = gpuGetMaxGflopsDeviceId();
	checkCudaErrors(cudaSetDevice(devID));
	checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devID));
	printf("GPU Device %d: \"%s\" with compute capability %d.%d\n", 
		devID, deviceProp.name, deviceProp.major, deviceProp.minor);*/
		
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