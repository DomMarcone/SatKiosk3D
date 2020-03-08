//Camera.h

#ifndef _CAMERA_H
#define _CAMERA_H

#include <linmath.h>

#define MAX_DRAW_DISTANCE 1800000.f

class Camera {
private :
	
	vec3 position;
	
	float yaw, tilt;
	
	float aspect;
	
	mat4x4 viewProjection;
		
	
public :
	Camera();
	~Camera();
	
	void update();
	
	void setViewSize(int width, int height);
	
	void setPosition(float x, float y, float z, bool update_transform=true);
	void setAngle(float y, float t, bool update_transform=true);
		
	float getYaw();
	float getTilt();
	
	void faceTarget(vec3 target);
	
	mat4x4 &getTransform();
};

#endif //_CAMERA_H
