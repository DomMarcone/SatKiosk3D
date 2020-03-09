# SatKiosk3D

A simple, OpenGL ES renderer for viewing TLE data efficiently.

## Building

SatKiosk3D requires CMake. It also requires the packages glfw3, libpng, and curl. Hardware must support at least OpenGL ES 3.1, or OpenGL 4.5. 
Download using : 
```bash
git clone https://github.com/DomMarcone/SatKiosk3D.git
```

Download the dependencies on Windows with :
```bash
vcpkg install glfw3 libpng curl
```

Download the dependencies on Linux with :
```bash
sudo apt-get install pkg-config glfw libpng libcurl4-openssl-dev
```
note : you could also use any variant of libcurl. Such as libcurl4-gnutls-dev, or libcurl4-nss-dev.

Enter the directory, and create an empty build directory, and enter it
```bash
cd SatKiosk3D
mkdir build
cd build
```

Build the project on Windows with :
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=%PATH_TO_VCPKG%\scripts\buildsystems\vcpkg.cmake
```

Build on linux with :
```bash
cmake ..
```
## Usage
See ``SatKiosk3D --help`` for the current list of supported options.
