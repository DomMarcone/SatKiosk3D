//test.cc

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <linmath.h>

#include <iostream>
#include <string>
#include <chrono>
#include <cmath>
#include <cstring>
#include <ctime>

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
#include <GPSTypes.h>
#include <GPSConvert.h>
#include <FetchTLE.h>
#include <RenderEarth.h>
#include <RenderTLE.h>
#include <RenderSun.h>
#include <RenderMoon.h>

#define DEFAULT_WINDOW_HEIGHT 540
#define DEFAULT_WINDOW_WIDTH 960

#define VERSION "0.01"

#define TLE_URL "http://www.celestrak.com/NORAD/elements/active.txt"
#define TLE_FILE "active.txt"

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
	float scrollAmount = 1000.f;
	
	float time_offset = 0;
	float time_increment = 60;
	std::string increment_s = "minutes";
	float time_flow = 1.f;
	int speed = 0;
	
	bool draw_rings = true;
	bool draw_sats = true;
} GraphicsState;

void DisplayTime(int offset){
	struct tm *ptm;
	time_t now, off;
	
	time(&now);
	
	ptm = localtime(&now);
	
	ptm->tm_sec += offset;
	
	off = mktime(ptm);
	ptm = localtime(&off);
	
	std::cout << "Time : " << 1 + ptm->tm_mon << "/" << ptm->tm_mday << "/" << 1900 + ptm->tm_year;
	std::cout << " " << ptm->tm_hour << ":" << ptm->tm_min << ":" << ptm->tm_sec << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;//No need to continue through each if block
	}
	
	if( key == GLFW_KEY_A && action == GLFW_PRESS){
		std::cout << "a key pressed." << std::endl;
		return;
	}
	
	if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
		GraphicsState.time_offset -= GraphicsState.time_increment;
		std::cout << "Time offset = " << 
			GraphicsState.time_offset/GraphicsState.time_increment <<
			" " << GraphicsState.increment_s << std::endl;
		DisplayTime(GraphicsState.time_offset);
		return;
	}
	
	if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
		GraphicsState.time_offset += GraphicsState.time_increment;
		std::cout << "Time offset = " << 
			GraphicsState.time_offset/GraphicsState.time_increment << 
			" " << GraphicsState.increment_s << std::endl;
		DisplayTime(GraphicsState.time_offset);
		return;
	}
	
	if(key == GLFW_KEY_R && action == GLFW_PRESS){
		GraphicsState.time_offset -= GraphicsState.time_offset;
		std::cout << "Time offset has been reset." << std::endl;
		return;
	}
	
	if(key == GLFW_KEY_S && action == GLFW_PRESS){
		GraphicsState.time_increment = 1;
		GraphicsState.increment_s = "seconds";
		std::cout << "Time increment is seconds." << std::endl;
		return;
	}
	
	if(key == GLFW_KEY_M && action == GLFW_PRESS){
		GraphicsState.time_increment = 60;
		GraphicsState.increment_s = "minutes";
		std::cout << "Time increment is minutes." << std::endl;
		return;
	}
	
	if(key == GLFW_KEY_H && action == GLFW_PRESS){
		GraphicsState.time_increment = 60*60;
		GraphicsState.increment_s = "hours";
		std::cout << "Time increment is hours." << std::endl;
		return;
	}
	
	if(key == GLFW_KEY_D && action == GLFW_PRESS){
		GraphicsState.time_increment = 24*60*60;
		GraphicsState.increment_s = "days";
		std::cout << "Time increment is days." << std::endl;
		return;
	}
	
	if(key == GLFW_KEY_W && action == GLFW_PRESS){
		GraphicsState.time_increment = 7*24*60*60;
		GraphicsState.increment_s = "weeks";
		std::cout << "Time increment is weeks." << std::endl;
		return;
	}
	
	if(key == GLFW_KEY_Y && action == GLFW_PRESS){
		//GraphicsState.time_increment = 31558149.504;
		GraphicsState.time_increment = 365*24*60*60;
		GraphicsState.increment_s = "years";
		std::cout << "Time increment is years." << std::endl;
		return;
	}
	
	if(key == GLFW_KEY_COMMA && action == GLFW_PRESS){
		GraphicsState.speed--;
		if(GraphicsState.speed<0)
			GraphicsState.time_flow = -abs(GraphicsState.time_flow);
		std::cout << "Time multiplier : *" << 
			pow(2.0, abs(GraphicsState.speed)) * GraphicsState.time_flow << std::endl;
		return;
	}
	
	if(key == GLFW_KEY_PERIOD && action == GLFW_PRESS){
		GraphicsState.speed++;
		if(GraphicsState.speed>=0)
			GraphicsState.time_flow = abs(GraphicsState.time_flow);
		std::cout << "Time multiplier : *" << 
			pow(2.0, abs(GraphicsState.speed)) * GraphicsState.time_flow << std::endl;
		return;
	}
	
	if(key == GLFW_KEY_1 && action == GLFW_PRESS){
		GraphicsState.draw_rings = !GraphicsState.draw_rings;
		std::cout << "Show rings : " << (GraphicsState.draw_rings ? "true" : "false") << std::endl;
		return;
	}

	if(key == GLFW_KEY_2 && action == GLFW_PRESS){
		GraphicsState.draw_sats = !GraphicsState.draw_sats;
		std::cout << "Show sats : " << (GraphicsState.draw_sats ? "true" : "false") << std::endl;
		return;
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
	GraphicsState.camAttrib.gps.altitude -= (yoffset * GraphicsState.scrollAmount) * log(GraphicsState.camAttrib.gps.altitude/500.f);
	
	if(GraphicsState.camAttrib.gps.altitude < 100.f)
		GraphicsState.camAttrib.gps.altitude = 100.f;
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
	RenderEarth *re;
	RenderTLE *rt;
	RenderSun *rs;
	RenderMoon *rm;
	
	std::string tle_url = TLE_URL, tle_file = TLE_FILE;
	
	bool show_gl_info = false;
	bool multisample = false;
	
	double hour_offset;
	
	{
		time_t raw_time, gm_time;
		struct tm *raw_tm;
		
		time(&raw_time);
		
		raw_tm = gmtime(&raw_time);
		gm_time = mktime(raw_tm);
		
		hour_offset = difftime(gm_time, raw_time)/(60*60);
		if(raw_tm->tm_isdst)hour_offset += 1.0;
	}
	
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
			
			continue;
		}
		
		if(strcmp(argument, "-v")==0){
			std::cout << "Version : " << VERSION << std::endl;
			std::cout << "Compiled on " << __DATE__ << " " << __TIME__ << std::endl;
			
			continue;
		}
		
		if(strcmp(argument, "-g")==0){
			show_gl_info = true;
			continue;
		}
		
		if(strcmp(argument, "-d")==0){
			if(i<argc-1){
				++i;
				
				tle_url = argv[i];
				tle_file = tle_url.substr( tle_url.find_last_of("/\\")+1 );
				
				std::cout << "Downloading TLE from " << tle_url << std::endl;
				std::cout << "Saving to " << tle_file << std::endl;
				FetchTLE(tle_url);
			} else {
				std::cout << "No file specified! Defaulting to " << TLE_URL << std::endl;
			}
			continue;
		}
		
		if(strcmp(argument, "-l")==0){
			if(i<argc-1){
				++i;
				tle_file = argv[i];
			} else {
				std::cout << "No file specified! Defaulting to " << TLE_FILE << std::endl;
			}
			continue;
		}
		
		if(strcmp(argument, "-a")==0){
			std::cout << "Multisampling enabled." << std::endl;
			glfwWindowHint( GLFW_SAMPLES, 4 );
			multisample = true;
		}
		
		if(strcmp(argument, "-h")==0 ||
			strcmp(argument, "--help")==0
			){
			
			std::cout << "SatKiosk3D" << std::endl;
			std::cout << "Currently supported options :" << std::endl;
			std::cout << " -f        : Run in full screen." << std::endl;
			std::cout << " -v        : Display version information." << std::endl;
			std::cout << " -g        : Display OpenGL information." << std::endl;
			std::cout << " -a        : Enable multisampling." << std::endl;
			std::cout << " -d [url]  : Download a TLE file from a URL." << std::endl;
			std::cout << " -l [file] : Load a local TLE file." << std::endl;
			std::cout << " -h --help : Display this." << std::endl;
			continue;
		}
	}
	
	if(GraphicsState.monitor){//Run in fullscreen
		const GLFWvidmode* mode = glfwGetVideoMode(GraphicsState.monitor);
		GraphicsState.width = mode->width;
		GraphicsState.height = mode->height;
		
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
		
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	
	glViewport(0, 0,
		GraphicsState.width,
		GraphicsState.height
	);
	
	if( multisample ){
		glEnable(GL_MULTISAMPLE);
	}
	
	GraphicsState.camera = new Camera();
	GraphicsState.camera->setViewSize(
		GraphicsState.width,
		GraphicsState.height
	);
	
	GraphicsState.camAttrib.gps.altitude = 12000.f;
	GraphicsState.camAttrib.gps.longitude = 0.f;
	GraphicsState.camAttrib.gps.latitude = 0.f;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	re = new RenderEarth(GraphicsState.camera);
	rt = new RenderTLE(GraphicsState.camera);
	rs = new RenderSun(GraphicsState.camera);
	rm = new RenderMoon(GraphicsState.camera);
	
	if(!rt->loadFile( tle_file )){
		std::cout << "Attempting to download TLE Data." << std::endl;
		FetchTLE( tle_url );
		if(!rt->loadFile( tle_file )){
			std::cout << "Unable to aquire " << tle_url << std::endl;
		}
	}
	rt->compute(std::chrono::system_clock::now(), true );
	
	while(!glfwWindowShouldClose(GraphicsState.window)){
		//View Loop
		vec3 target = {0.f, 0.f, 0.f};
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
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
					delta[i] *= GraphicsState.scaleMouse / log(GraphicsState.camAttrib.gps.altitude/500.0) ;
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
			std::chrono::system_clock::time_point now = 
				std::chrono::system_clock::now();
			
			if(GraphicsState.speed)
				GraphicsState.time_offset += (pow(2.0, abs(GraphicsState.speed))*GraphicsState.time_flow)/60.f;
			
			now += std::chrono::milliseconds((int)(fmod(GraphicsState.time_offset,1.0)*1000.f));
			now += std::chrono::seconds((int)(fmod(GraphicsState.time_offset,60.0)));
			now += std::chrono::minutes((int)(fmod(GraphicsState.time_offset,3600.0)/60.0));
			now += std::chrono::hours((int)(GraphicsState.time_offset/3600.0 + hour_offset));
			
			re->setTime(now);
			rm->setTime(now);
		
			rt->compute(now, true);
		}
		
		rs->setDirection(re->getSunDirection());
		rm->setSunDirection(re->getSunDirection());
		
		rs->draw();
		rm->draw();
		re->draw();
		rt->draw(GraphicsState.draw_rings, GraphicsState.draw_sats);
		
		glfwSwapBuffers(GraphicsState.window);
        glfwPollEvents();
	}
	
	delete rs;
	delete rt;
	delete re;
	delete GraphicsState.camera;
	
	glFlush();
	
	glfwDestroyWindow(GraphicsState.window);
	glfwTerminate();
		
	std::cout << "exiting..." << std::endl;
	return 0;
}

