#include "main.h"

using namespace std;

// Initialize all the classes the engine needs.
void init()
{
	camera = new CameraFPS(window_width, window_height);
	fpsTimer = new Timer(1.0f);
	fpsTimer->start();
	scene = new Scene(camera);
	scene->load();
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

// Function, arrow and other special keys.
void keyboardSpecial(int key, int x, int y)
{
	//camera->keyboardSpecial(key);
	//scene->keyboardSpecial(key);
}

// Special key up event.
void keyboardSpecialUp(int key, int x, int y)
{
	//camera->keyboardSpecialUp(key);
	//scene->keyboardSpecialUp(key);
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

int main(int argc, char** argv)
{
	// Initialize glut and create window
	glutInit(&argc, argv);
	glutInitWindowPosition(-1, -1);
	glutInitWindowSize(window_width, window_height);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(g_windowTitle.c_str());

	// Enable fullscreen mode if arg provided
	stringstream ss;
	if (argc == 2)
	{
		ss << argv[1] << "";
	}

	if (ss.str() == "-fullscreen")
	{
		glutGameModeString("1280x800:32@60");
		glutEnterGameMode();
	}

	// Initialize glew and our classes
	glewInit();
	printf("OpenGL version: %s \nGLEW version: %s \n", 
		glGetString(GL_VERSION), glewGetString(GLEW_VERSION));

	init();

	// Callback Functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(mouseMovePassive);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(keyboardSpecial);
	glutSpecialUpFunc(keyboardSpecialUp);

	glutMainLoop();

	return 0;
}