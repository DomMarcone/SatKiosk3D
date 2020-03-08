# SatKiosk3D

A simple, OpenGL ES renderer for viewing TLE data efficiently.

## Building

SatKiosk3D requires CMake, and VCPKG. It also requires the packages glfw3, libpng, and curl. Hardware must support at least OpenGL ES 3.0, or OpenGL 4.3. 
Download using : 
```bash
git clone https://github.com/DomMarcone/SatKiosk3D.git
```

Download the dependencies
```bash
vcpkg install glfw3 libpng curl
```

Enter the directory, and create an empty build directory, and enter it
```bash
cd SatKiosk3D
mkdir build
cd build
```

Build the project with
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=%PATH_TO_VCPKG%\scripts\buildsystems\vcpkg.cmake
```

## Usage
See ``SatKiosk3D --help`` for the current list of supported options.
