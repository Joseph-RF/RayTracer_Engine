#include <scenesaver.hpp>

namespace SceneSaver {
void saveScene(const App& app) {

    std::ofstream outfile(RESOURCES_PATH "/save_data/test.txt");

    for (unsigned int i = 0; i < app.game_objects.size(); ++i) {

        // Game object number
        outfile << std::to_string(i) << " ";

        outfile << app.game_objects[i]->dataToString();
    }
}
void loadScene(App& app) {
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

    if (infile.is_open()) {
        while (std::getline(infile, str)) {
            std::string temp;
            std::stringstream ss(str);

            ss >> temp; // Get rid of the number

            ss >> temp; // Get object class type

            unsigned int index = new_object_list.size();

            if (temp == "Cube") {
                new_object_list.push_back(createCubeFromData(str));
            } else if (temp == "Arrow") {
                new_object_list.push_back(createArrowFromData(str));
            } else if (temp == "HollowCylinder") {
                new_object_list.push_back(createHollowCylinderFromData(str));
            } else if (temp == "Sphere") {
                new_object_list.push_back(createSphereFromData(str));
            } else {
                std::cout << "Unexpected object type found in save data" << std::endl;
            }
            // Check if a light was added
            if (new_object_list[index]->light) {
                num_lights++;
            }
        }
        infile.close();

        app.game_objects = new_object_list;
        app.num_lights   = num_lights;
        // Ensure pointers are reset
        app.resetObjectPointers();
    } else {
        std::cout << "Unable to open file" << std::endl;
        return;
    }
}
} // namespace SceneSaver
