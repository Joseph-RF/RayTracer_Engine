#pragma once

#include <iostream>
#include <fstream> 

#include <engine.hpp>

class Engine;

namespace SceneSaver {
	void saveScene(Engine& engine);

	void loadScene(Engine& engine);
}