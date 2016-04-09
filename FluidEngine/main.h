#include <iostream>
#include <sstream>
#include <thread>
#include "Includes.h"
#include "Scene.h"

std::string g_windowTitle = "FluidEngine";
std::string g_mediaDirectory = "../FluidEngine/media/";

// Window display area.
int window_width = 1280;
int window_height = 720;

Scene* scene;