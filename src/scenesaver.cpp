#include <scenesaver.hpp>

namespace SceneSaver {
void saveScene(Engine& engine) {

    std::ofstream outfile(RESOURCES_PATH "/save_data/test.txt");

    for (unsigned int i = 0; i < engine.game_objects.size(); ++i) {

        // Game object number
        outfile << std::to_string(i) << " ";

        outfile << engine.game_objects[i]->dataToString();
    }
}
void loadScene(Engine& engine) {
    // Several assumptions being made in the implementation of this function:
    // All objects are cubes
    // Cubes have the following data members: position, orientation, size, colour
    // , shininess, name and a Light pointer
    // Light class has the following data members: ambient, diffuse, specular,
    // constant, linear and quadratic

    std::vector<std::shared_ptr<GameObject>> new_object_list;
    unsigned int num_lights = 0;

    std::ifstream infile(RESOURCES_PATH "save_data/test.txt");
    std::string str;
    Light temp_light(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    if (infile.is_open()) {
        while (std::getline(infile, str)) {
            std::string temp;
            std::stringstream ss(str);

            ss >> temp; // Get rid of the number

            ss >> temp; // Get object class type

            unsigned int index = new_object_list.size();

            if (temp == "Cube") {
                new_object_list.push_back(createCubeFromData(str));
                if (new_object_list[index]->light) {
                    num_lights++;
                }
            } else {
                std::cout << "Unexpected object type found in save data" << std::endl;
            }
        }
        infile.close();

        engine.game_objects = new_object_list;
        engine.num_lights   = num_lights;
        // Ensure pointers are reset
        engine.resetObjectPointers();
    } else {
        std::cout << "Unable to open file" << std::endl;
        return;
    }
}
} // namespace SceneSaver
