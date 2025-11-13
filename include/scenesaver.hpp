#pragma once

#include <fstream>
#include <iostream>

#include <app.hpp>

class App;

namespace SceneSaver {
void saveScene(const App& app);

void loadScene(App& app);
} // namespace SceneSaver
