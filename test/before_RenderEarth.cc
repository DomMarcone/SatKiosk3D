//test.cc

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <linmath.h>

#include <iostream>
#include <string>
#include <cmath>
#include <cstring>

#include <Format3D.h>
#include <LoadText.h>
#include <PlyToFormat3D.h>
#include <Image.h>
#include <LoadImagePNG.h>
#include <Camera.h>
#include <CompileShader.h>
#include <LinkShaders.h>
#include <DrawFormat3D.h>
#include <ImageToTexture.h>
#include <Camera.h>
#include <EarthTransform.h>
#include <GPSTypes.h>
#include <GPSConvert.h>
#include <FetchTLE.h>

#define DEFAULT_WINDOW_HEIGHT 540
#define DEFAULT_WINDOW_WIDTH 960

#define VERSION "0.0"

//Viewport stuff
struct {
	GLFWwindow *window;
	GLFWmonitor *monitor = NULL;
	
	unsigned int width = DEFAULT_WINDOW_WIDTH;
	unsigned int height = DEFAULT_WINDOW_HEIGHT;
	
	Camera *camera;
	struct {
		gps_t gps;
		xyz_t pos;
	} camAttrib;
	
	bool mouse_down = false;
	bool been_clicked = false;
	
	//Mouse Control Attributes
	float scaleMouse = 0.25f;
	float scrollAmount = 500.f;
	
	int time_offset = 0;
} GraphicsState;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	
	if( key == GLFW_KEY_A && action == GLFW_PRESS){
		std::cout << "a key pressed." << std::endl;
	}
	
	if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
		GraphicsState.time_offset -= 60*60;
		std::cout << "Time offset = " << GraphicsState.time_offset << std::endl;
	}
	
	if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
		GraphicsState.time_offset += 60*60;
		std::cout << "Time offset = " << GraphicsState.time_offset << std::endl;
	}
	
	if(key == GLFW_KEY_D && action == GLFW_PRESS){
		GraphicsState.time_offset += 60*60*24;
		std::cout << "Time offset = " << GraphicsState.time_offset << std::endl;
	}
}

void mouse_callback(GLFWwindow *window, int button, int action, int mods){
	
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		GraphicsState.mouse_down = true;
	}
	
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
		GraphicsState.mouse_down = false;
		GraphicsState.been_clicked = false;
	}
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset){
	GraphicsState.camAttrib.gps.altitude -= yoffset * GraphicsState.scrollAmount;
}

void resize_callback(GLFWwindow *window, int width, int height){	
	GraphicsState.width = width;
	GraphicsState.height = height;
	
	glViewport(0, 0, 
		width, 
		height
	);
	
	GraphicsState.camera->setViewSize(width, height);
}

void error_callback(int error, const char* description){
	std::cout << "Error: " << description << std::endl;
}

int main(int argc, char **argv){
	/*
	GLuint sFrag, sVert, program, earth_texture_diff, earth_texture_night;
	GLint uViewProjection, uModel, uSunDirection;
	GLint uDiff, uNight;
	
	EarthTransform et;
	et.setRotationOffset(-3.1415928);
	
	Format3D *earth_model = PlyToFormat3D(
			LoadText("res/models/earth.ply")
		);
	std::cout << "earth model loaded." << std::endl;
	
	Image earth_image_diff, earth_image_night;
	LoadImagePNG(earth_image_diff, "res/images/map_diff.png");
	LoadImagePNG(earth_image_night, "res/images/map_night.png");
	std::cout << "earth images loaded." << std::endl;
	*/
	bool show_gl_info = false;
	
	if(!glfwInit()){
		std::cout << "Error initializing glfw." << std::endl;
		return -1;
	}
	
	glfwSetErrorCallback(error_callback);
		
	glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1);
	

	//parse arguments
	for(int i=0; i<argc; ++i){
		char *argument = argv[i];
		
		if(strcmp(argument, "-f")==0){
			std::cout << "Running in fullscreen" << std::endl;
			GraphicsState.monitor = glfwGetPrimaryMonitor();
		}
		
		if(strcmp(argument, "-v")==0){
			std::cout << "Version : " << VERSION << std::endl;
			std::cout << "Compiled on " << __DATE__ << " " << __TIME__ << std::endl;
			
		}
		
		if(strcmp(argument, "-g")==0){
			show_gl_info = true;
		}
		
		if(strcmp(argument, "-d")==0){
			std::cout << "Downloading TLE Data." << std::endl;
			FetchTLE("http://www.celestrak.com/NORAD/elements/active.txt");
		}
	}

	GraphicsState.window = glfwCreateWindow(
		GraphicsState.width, 
		GraphicsState.height, "SatKiosk3D", 
		GraphicsState.monitor, NULL);
	
	if(!GraphicsState.window){
		std::cout << "Error generating window." << std::endl;
		return -1;
	}
	
	glfwSetKeyCallback(GraphicsState.window, key_callback);
	glfwSetWindowSizeCallback(GraphicsState.window, resize_callback);
	glfwSetMouseButtonCallback(GraphicsState.window, mouse_callback);
	glfwSetScrollCallback(GraphicsState.window, scroll_callback);
	
	glfwMakeContextCurrent(GraphicsState.window);
	
	gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval(1);
	
	if(show_gl_info){
		GLint temp[2];
		
		glGetIntegerv(GL_MAJOR_VERSION ,&temp[0]);
		glGetIntegerv(GL_MINOR_VERSION ,&temp[1]);
		
		std::cout << "OpenGL version : " << temp[0] << "." << temp[1] << std::endl;
		
		std::cout << "GL_VENDOR                   : " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "GL_VERSION                  : " << glGetString(GL_VERSION) << std::endl;
		std::cout << "GL_RENDERER                 : " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "GL_SHADING_LANGUAGE_VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	}
		
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	
	glViewport(0, 0,
		GraphicsState.width,
		GraphicsState.height
	);
	
	/*
	//Earth Shader
	sVert = CompileShader(LoadText("res/shaders/earth_vert.glsl"), GL_VERTEX_SHADER);
	sFrag = CompileShader(LoadText("res/shaders/earth_frag.glsl"), GL_FRAGMENT_SHADER);
	
	program = LinkShaders(sVert, sFrag);
	
	glUseProgram(program);
	uDiff = glGetUniformLocation(program, "uDiff");
	uNight = glGetUniformLocation(program, "uNight");
	uSunDirection = glGetUniformLocation(program, "uSunDirection");
	uViewProjection = glGetUniformLocation(program, "uViewProjection");
	uModel = glGetUniformLocation(program, "uModel");
	
	DrawFormat3D earth_drawer(program, earth_model);
	
	earth_texture_diff = ImageToTexture(earth_image_diff);
	earth_texture_night = ImageToTexture(earth_image_night);
	*/
	GraphicsState.camera = new Camera();
	GraphicsState.camera->setViewSize(
		GraphicsState.width,
		GraphicsState.height
	);
	
	GraphicsState.camAttrib.gps.altitude = 12000.f;
	GraphicsState.camAttrib.gps.longitude = 0.f;
	GraphicsState.camAttrib.gps.latitude = 0.f;
	
	while(!glfwWindowShouldClose(GraphicsState.window)){
		//View Loop
		vec3 target = {0.f, 0.f, 0.f};
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(program);
		
		//Handle Mouse Input
		if(GraphicsState.mouse_down){
			static gps_t gps_click_position;
			static double clickPosition[2];
			
			double mousePosition[2];
			
			glfwGetCursorPos(
				GraphicsState.window, 
				&mousePosition[0],
				&mousePosition[1]
			);
			
			if(GraphicsState.been_clicked){
				vec2 delta;
				
				for(int i=0; i<2; ++i){
					delta[i] = (float)mousePosition[i] - clickPosition[i];
					delta[i] *= GraphicsState.scaleMouse;
				}
				
				delta[0] += gps_click_position.longitude;
				delta[1] += gps_click_position.latitude;
				
				GraphicsState.camAttrib.gps.longitude = delta[0];
				GraphicsState.camAttrib.gps.latitude = delta[1];

				//Limits
				if(GraphicsState.camAttrib.gps.latitude > 90.f)
					GraphicsState.camAttrib.gps.latitude = 90.f;

				if(GraphicsState.camAttrib.gps.latitude < -90.f)
					GraphicsState.camAttrib.gps.latitude = -90.f;
				
			} else {
				glfwGetCursorPos(
					GraphicsState.window, 
					&clickPosition[0],
					&clickPosition[1]
				);
				
				gps_click_position.latitude = GraphicsState.camAttrib.gps.latitude;
				gps_click_position.longitude = GraphicsState.camAttrib.gps.longitude;
				
				GraphicsState.been_clicked = true;
			}
		}
		
		to_xyz(
			GraphicsState.camAttrib.pos, 
			&GraphicsState.camAttrib.gps
		);
		
		GraphicsState.camera->setPosition(
			GraphicsState.camAttrib.pos[0], 
			GraphicsState.camAttrib.pos[1],
			GraphicsState.camAttrib.pos[2]
		);
		
		GraphicsState.camera->faceTarget(target);
		
		
		{
			time_t current_time;
			struct tm *gm_current_time;
			time(&current_time);
			gm_current_time = gmtime(&current_time);
			
			gm_current_time->tm_sec += GraphicsState.time_offset;
			
			current_time = mktime(gm_current_time);
				
			//et.setCurrentTime(current_time);
		}
		/*
		glUniformMatrix4fv(
			uModel, 
			1, 
			GL_FALSE, 
			(const GLfloat*) et.getTransform()
		);
		
		glUniformMatrix4fv(
			uViewProjection, 
			1, 
			GL_FALSE, 
			(const GLfloat*) GraphicsState.camera->getTransform()
		);

		glUniform3fv(uSunDirection, 1, et.getSunDirection());
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, earth_texture_diff);
		glUniform1i(uDiff, 0);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, earth_texture_night);
		glUniform1i(uNight, 1);
		
		earth_drawer.draw();
		*/
		glfwSwapBuffers(GraphicsState.window);
        glfwPollEvents();
	}
	
	//DeleteFormat3D(earth_model);
	//glDeleteProgram(program);
	
	delete GraphicsState.camera;
	
	glFlush();
	
	glfwDestroyWindow(GraphicsState.window);
	glfwTerminate();
		
	std::cout << "exiting..." << std::endl;
	return 0;
}

