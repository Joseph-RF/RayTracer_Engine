#pragma once

#include <fstream>
#include <iostream>

#include <engine.hpp>

class Engine;

namespace SceneSaver {
void saveScene(Engine& engine);

void loadScene(Engine& engine);
} // namespace SceneSaver
