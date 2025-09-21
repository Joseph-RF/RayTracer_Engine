<a id="readme-top"></a>

<br />
<div align="center">

  <h3 align="center">RayTracer Engine</h3>

  <p align="center">
    . 
    <br />
    <br />
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#built-with">Built With</a></li>
    <li><a href="#building-instructions">Building Instructions</a></li>
    <li><a href="#future-features">Future Features</a></li>
    <li><a href="#bugs-and-limitations">Bugs and Limitations</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

Engine allowing for scene creation for the purpose of generating a raytraced image. Raytracing aspect of the program can be found in this [project](https://github.com/Joseph-RF/RayTracer/).

The engine is currently being developed and as such is missing several features. A list of features to be added can be found below.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Built With

This project was built with the following third party dependencies:

* [OpenGL][OpenGL-url]
* [DearImGui](https://github.com/ocornut/imgui)
* [GLM](https://glm.g-truc.net/0.9.9/index.html)
* [GLFW](https://www.glfw.org/)
* [Glad](https://github.com/premake-libs/glad?tab=readme-ov-file)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- BUILDING INSTRUCTIONS -->
## Building Instructions

#### Building Requirements:
* CMake
* C++ Compiler

For Windows:
```bash
git clone https://github.com/Joseph-RF/RayTracer_Engine.git
cd RayTracer_Engine
mkdir build
cd .\build\
cmake ..
cmake --build .
.\Debug\main.exe
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- FUTURE FEATURES -->
## Future Features

* Materials
* Compatibility with the raytracer
* Add the raytracer to the project

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- BUGS AND LIMITATIONS -->
## Bugs and Limitations

This section is to list bugs or limitations of the project

* If window size is set to (1900, 1080), mouse picking fails if window is not made fullscreen (maximise).
* Using rotation gizmos occassionally results in nan values.
* No shadows, light passes through objects.
* Slow performance
* Unexplained crashes

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Project Link: [https://github.com/Joseph-RF/RayTracer_Engine](https://github.com/Joseph-RF/RayTracer_Engine)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

Find below a list of resources that were vital/helpful for the development of this project.

* [Learn OpenGL](https://learnopengl.com/)
* [songho.ca](https://www.songho.ca/opengl/gl_cylinder.html#:~:text=In%20order%20to%20draw%20the,side%20is%202%20%C3%97%20sectorCount.)
* [Underdisc](https://underdisc.net/index.html)
* [Interactive Technique in Three-dimensional-Scences](https://www.codeproject.com/Articles/35139/Interactive-Techniques-in-Three-dimensional-Scenes)
* [Ray Tracing in One Weekend Series](https://raytracing.github.io/)
* [README Template](https://github.com/othneildrew/Best-README-Template)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[OpenGL-url]: https://www.opengl.org/
[OpenGL-logo]: https://img.shields.io/badge/OpenGL-FFFFFF?style=for-the-badge&logo=opengl