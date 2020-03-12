cmake_minimum_required(VERSION 3.14)

project(SatKiosk3D)

set(CMAKE_CXX_STANDARD 17)

set(TEST_DIR test)
set(RES_DIR res)
set(DEPS_DIR deps)
set(SRC_DIR src)

#Packages
find_package(OpenGL REQUIRED)
if (WIN32)
	find_package(png REQUIRED)
	find_package(glfw3 CONFIG REQUIRED)
	find_package(Threads REQUIRED)
	find_package(CURL CONFIG REQUIRED)
else (WIN32)
	find_package(PkgConfig)
	find_package(PNG)
	find_package(glfw3 CONFIG)
	find_package(Threads)
	if( NOT PKGCONFIG_FOUND )
		find_package(CURL CONFIG)
	else( NOT PKGCONFIG_FOUND )
		pkg_check_modules(CURL REQUIRED IMPORTED_TARGET libcurl)
	endif( NOT PKGCONFIG_FOUND )
endif (WIN32)

include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${DEPS_DIR}/linmath/ )

#Create GLAD library
add_library(glad 
	${DEPS_DIR}/glad/src/glad.c
	${DEPS_DIR}/glad/include/glad/glad.h
	${DEPS_DIR}/glad/include/KHR/khrplatform.h
)
target_include_directories(glad PUBLIC ${DEPS_DIR}/glad/include/ )

if(UNIX)
	set(${CMAKE_CXX_COMPILER} /usr/bin/g++)
	set(${CMAKE_C_COMPILER} /usr/bin/gcc) 
endif(UNIX)

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
	${SRC_DIR}/RenderSun.h
	${SRC_DIR}/RenderSun.cc
	${SRC_DIR}/RenderTLE.h
	${SRC_DIR}/RenderTLE.cc
	${SRC_DIR}/TLEToMat4x4.h
	${SRC_DIR}/TLEToMat4x4.cc
	${SRC_DIR}/TLEToAngle.h
	${SRC_DIR}/TLEToAngle.cc
	${SRC_DIR}/LoadTLE.h
	${SRC_DIR}/LoadTLE.cc
	${SRC_DIR}/intrin_linmath.h
	${SRC_DIR}/Bin.h
	${SRC_DIR}/Bin.cc
	${SRC_DIR}/LoadBin.h
	${SRC_DIR}/LoadBin.cc
	${SRC_DIR}/SaveBin.h
	${SRC_DIR}/SaveBin.cc
	${SRC_DIR}/Format3DToBin.h
	${SRC_DIR}/Format3DToBin.cc
	${SRC_DIR}/BinToFormat3D.h
	${SRC_DIR}/BinToFormat3D.cc
	${SRC_DIR}/LoadFormat3D.h
	${SRC_DIR}/LoadFormat3D.cc
)

target_link_libraries(SatKiosk3DLib PUBLIC ${PNG_LIBRARY})
target_link_libraries(SatKiosk3DLib PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(SatKiosk3DLib PUBLIC ${OPENGL_LIBRARY})
target_link_libraries(SatKiosk3DLib PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(SatKiosk3DLib PUBLIC glad )
target_link_libraries(SatKiosk3DLib PRIVATE glfw )
if (WIN32)
	target_link_libraries(SatKiosk3DLib PRIVATE CURL::libcurl )
else (WIN32)
	if( CURL_FOUND )
		target_link_libraries(SatKiosk3DLib PRIVATE ${CURL_LIBRARIES})
	else( CURL_FOUND )
		target_compile_options(SatKiosk3DLib PRIVATE -lcurl -lm)
		set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -lcurl -lm")
		set(${CMAKE_C_FLAGS} "${CMAKE_C_FLAGS} -lcurl -lm")
	endif( CURL_FOUND )
	target_link_libraries(SatKiosk3DLib PUBLIC ${CMAKE_DL_LIBS})
	target_include_directories(SatKiosk3DLib PRIVATE ${CURL_INCLUDE_DIR})
endif(WIN32)
target_include_directories(SatKiosk3DLib PUBLIC ${PNG_INCLUDE_DIR} )
target_include_directories(SatKiosk3DLib PUBLIC ${SRC_DIR} )

#intrinsics
if(WIN32)
	#nothing to do here
else(WIN32)
	if( ${CMAKE_SYSTEM_PROCESSOR} MATCHES x86_64 )
		target_compile_options(SatKiosk3DLib PRIVATE -mfpmath=sse -mmmx -msse -msse2 -msse3)
		set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -mfpmath=sse -mmmx -msse -msse2 -msse3")
		set(${CMAKE_C_FLAGS} "${CMAKE_C_FLAGS} -mfpmath=sse -mmmx -msse -msse2 -msse3")
	elseif ( ${CMAKE_SYSTEM_PROCESSOR} MATCHES x86_64 == arm )
		target_compile_options(SatKiosk3DLib PRIVATE -mfpu=neon -mfloat-abi=softfp)
		set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -mfpu=neon -mfloat-abi=softfp")
		set(${CMAKE_C_FLAGS} "${CMAKE_C_FLAGS} -mfpu=neon -mfloat-abi=softfp")
	endif ()
endif(WIN32)

add_executable(SatKiosk3D ${TEST_DIR}/test.cc)
target_link_libraries(SatKiosk3D PUBLIC SatKiosk3DLib)

if( NOT CMAKE_BUILD_TYPE )
	set(CMAKE_BUILD_TYPE Debug)
endif( NOT CMAKE_BUILD_TYPE )

if(WIN32)
	set(RES_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/)
else(WIN32)
	set(RES_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif(WIN32)

#copy the res directory to the output
file(
	COPY ${CMAKE_CURRENT_SOURCE_DIR}/${RES_DIR}
	DESTINATION ${RES_OUT_DIR}
)
