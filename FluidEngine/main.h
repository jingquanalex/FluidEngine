#include <iostream>
#include <sstream>
#include <thread>
#include "Includes.h"
#include "Scene.h"

std::string g_windowTitle = "FluidEngine b1";
std::string g_mediaDirectory = "../FluidEngine/media/";

// Window display area.
int window_width = 1280;
int window_height = 800;

Scene* scene;