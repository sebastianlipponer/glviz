# GLviz
[![Build badge](https://github.com/sebastianlipponer/glviz/actions/workflows/build.yml/badge.svg)](https://github.com/sebastianlipponer/glviz/actions?workflow=build) [![license](https://img.shields.io/github/license/sebastianlipponer/glviz)](https://github.com/sebastianlipponer/glviz/blob/master/COPYING.MIT)

GLviz is a small collection of C++ classes and GLSL shaders to facilitate the development of OpenGL demos. It is built on top of [CMake](http://www.cmake.org/), [SDL](http://libsdl.org), [GLEW](http://glew.sourceforge.net), [Eigen](http://eigen.tuxfamily.org/), and [ImGui](https://github.com/ocornut/imgui) and requires at least OpenGL 3.3. GLviz has been tested on a NVIDIA GTX 1080 Ti GPU using driver version 436.02 on Windows 10 (compiled with MSVC 2019) and 418.74 on Linux (compiled with GCC 8.3). It includes a script to easily download and build all external dependencies in the project directory to make compilation on either Windows or Linux as simple and convenient as possible.

## Build

Before running CMake run either build-extern.cmd or build-extern.sh to download and build the necessary external dependencies in the .extern directory.

## Features

* Camera with trackball navigation.
* Shader management.
* Triangle mesh shader (supports flat or Phong shading and an optional high-quality wireframe<sup>1</sup> overlay).
* Sphere shader (for the sake of performance not perspectively correct spheres).
* Supports embedding of shaders in the executable.

## Screenshots

[![](http://sebastianlipponer.github.io/glviz/dragon_thumbnail.png)](http://sebastianlipponer.github.io/glviz/dragon.png)[![](http://sebastianlipponer.github.io/glviz/dragon_wireframe_thumbnail.png)](http://sebastianlipponer.github.io/glviz/dragon_wireframe.png)[![](http://sebastianlipponer.github.io/glviz/dragon_spheres_thumbnail.png)](http://sebastianlipponer.github.io/glviz/dragon_spheres.png)

## Documentation

Currently there is no documentation, but the code includes an example. From the source code it should become apparent how GLviz is intended to be used.

## References

[1] Bærentzen, J. A., Munk-Lund, S., Gjøl, M., Larsen, B. D.: **Two Methods for Antialiased Wireframe Drawing with Hidden Line Removal**. In Proceedings of the Spring Conference in Computer Graphics, 2008.
