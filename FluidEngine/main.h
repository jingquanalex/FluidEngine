#include <iostream>
#include <sstream>
#include <thread>
#include "Includes.h"
#include "Timer.h"
#include "CameraFPS.h"
#include "CameraTarget.h"
#include "Scene.h"

std::string g_windowTitle = "FluidEngine b1";
std::string g_mediaDirectory = "../FluidEngine/media/";

// Desired content display area, which maintains its ratio even if the window area is changed.
// For Camera's ortho matrix but unused at the moment.
float viewport_width = 1280;
float viewport_height = 720;

// Window display area.
int window_width = 1280;
int window_height = 800;

// Desired simulation step in ms.
const float maxframeTime = 1 / 4.0f;
const float dt = 1 / 60.0f;
float accumulator = 0.0f;
int fps = 0;
int currentTime, previousTime = 0;

// Framework classes
Timer* fpsTimer;
Camera* camera;
Scene* scene;