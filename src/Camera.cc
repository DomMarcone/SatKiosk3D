//Camera.cc

#include "Camera.h"

#include <linmath.h>
#include <intrin_linmath.h>

#include <cmath>

Camera::Camera(){
	aspect = 1.f;
	
	for(int i=0; i<3; ++i){
		position[i] = 0.f;
	}
	
	yaw = 0.f;
	tilt = 0.f;
}

Camera::~Camera(){
	//TODO : This function
}

void  Camera::setViewSize(int width, int height){
	aspect = (float)width/height;
}


void Camera::update(){
	mat4x4 view, projection;
	mat4x4 yaw_m, tilt_m, rot, trans;
	
	mat4x4_identity(view);
	//mat4x4_identity(yaw_m);
	
	mat4x4_rotate_X(tilt_m, view, tilt);
	mat4x4_rotate_Y(rot, tilt_m, yaw);
	
	mat4x4_translate(trans, -position[0], -position[1], -position[2]);
	
	intrin_mat4x4_mul(view, rot, trans);
	
	mat4x4_perspective(projection, 3.14159f/3.f, aspect, 100.f, MAX_DRAW_DISTANCE);
	
	intrin_mat4x4_mul(viewProjection, projection, view);
}

void Camera::setPosition(float x, float y, float z, bool update_transform){
	
	position[0] = x;
	position[1] = y;
	position[2] = z;
	
	if(update_transform)
		update();
}

void Camera::setAngle(float y, float t, bool update_transform){
	
	yaw = y;
	tilt = t;
	
	if(update_transform)
		update();
}

float Camera::getYaw(){ return yaw; }

float Camera::getTilt(){ return tilt; }

float Camera::getAspect(){ return aspect; }

void Camera::faceTarget(vec3 target){
	vec3 vectorToTarget;
	float distanceToTarget;
		
	vec3_sub(vectorToTarget, target, position);
		
	distanceToTarget = vec3_len(vectorToTarget);
	
	yaw = 3.14159265f + atan2(vectorToTarget[0], vectorToTarget[2]);
	
	tilt = asin( -vectorToTarget[1]/distanceToTarget );
	
	update();
}

mat4x4 &Camera::getTransform(){ return viewProjection; }
	
