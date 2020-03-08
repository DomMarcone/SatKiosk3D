cmake_minimum_required(VERSION 3.1.3)

project(SatKiosk3D)

set(CMAKE_CXX_STANDARD 17)

set(TEST_DIR test)
set(RES_DIR res)
set(DEPS_DIR deps)
set(SRC_DIR src)

#Required packages
find_package(png REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(CURL CONFIG REQUIRED)

include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${DEPS_DIR}/linmath/ )

#Create GLAD library
add_library(glad 
	${DEPS_DIR}/glad/src/glad.c
	${DEPS_DIR}/glad/include/glad/glad.h
	${DEPS_DIR}/glad/include/KHR/khrplatform.h
)
target_include_directories(glad PUBLIC ${DEPS_DIR}/glad/include/ )

add_library(SatKiosk3DLib
	${SRC_DIR}/Camera.h
	${SRC_DIR}/Camera.cc
	${SRC_DIR}/Format3D.h
	${SRC_DIR}/Format3D.cc
	${SRC_DIR}/Image.h
	${SRC_DIR}/Image.cc
	${SRC_DIR}/LoadImagePNG.h
	${SRC_DIR}/LoadImagePNG.cc
	${SRC_DIR}/LoadText.h
	${SRC_DIR}/LoadText.cc
	${SRC_DIR}/PlyToFormat3D.h
	${SRC_DIR}/PlyToFormat3D.cc
	${SRC_DIR}/CompileShader.h
	${SRC_DIR}/CompileShader.cc
	${SRC_DIR}/DrawFormat3D.h
	${SRC_DIR}/DrawFormat3D.cc
	${SRC_DIR}/ImageToTexture.h
	${SRC_DIR}/ImageToTexture.cc
	${SRC_DIR}/LinkShaders.h
	${SRC_DIR}/LinkShaders.cc
	${SRC_DIR}/EarthTransform.h
	${SRC_DIR}/EarthTransform.cc
	${SRC_DIR}/GPSConvert.h
	${SRC_DIR}/GPSConvert.cc
	${SRC_DIR}/GPSTypes.h
	${SRC_DIR}/FetchTLE.h
	${SRC_DIR}/FetchTLE.cc
	${SRC_DIR}/TLEType.h
	${SRC_DIR}/RenderEarth.h
	${SRC_DIR}/RenderEarth.cc
	${SRC_DIR}/RenderTLE.h
	${SRC_DIR}/RenderTLE.cc
	${SRC_DIR}/TLEToMat4x4.h
	${SRC_DIR}/TLEToMat4x4.cc
	${SRC_DIR}/TLEToAngle.h
	${SRC_DIR}/TLEToAngle.cc
	${SRC_DIR}/LoadTLE.h
	${SRC_DIR}/LoadTLE.cc
)

target_link_libraries(SatKiosk3DLib PUBLIC ${PNG_LIBRARY})
target_link_libraries(SatKiosk3DLib PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(SatKiosk3DLib PUBLIC ${OPENGL_LIBRARY})
target_link_libraries(SatKiosk3DLib PUBLIC glad )
target_link_libraries(SatKiosk3DLib PRIVATE glfw )
target_link_libraries(SatKiosk3DLib PRIVATE CURL::libcurl )
target_include_directories(SatKiosk3DLib PUBLIC ${PNG_INCLUDE_DIR} )
target_include_directories(SatKiosk3DLib PUBLIC ${SRC_DIR} )

add_executable(SatKiosk3D ${TEST_DIR}/test.cc)
target_link_libraries(SatKiosk3D PUBLIC SatKiosk3DLib)

if( NOT CMAKE_BUILD_TYPE )
	set(CMAKE_BUILD_TYPE Debug)
endif( NOT CMAKE_BUILD_TYPE )

#copy the res directory to the output
file(
	COPY ${CMAKE_CURRENT_SOURCE_DIR}/${RES_DIR}
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/
)