#include "Camera.h"

#define THIRD_PERSON 0
#define FINISH_CAM 1

#define DEG2RAD(x) float((x)*M_PI/180.0) 

// Create the camera from the bike position
Camera::Camera(Vehicle *v) {
	vehicleObj = v;
	cameraMode = THIRD_PERSON;
	up = glm::vec3(0.0, 1.0, 0.0);
	cameraPos = glm::vec3(vehicleObj->getPos()[0] + 0.0, vehicleObj->getPos()[1] + 7.0, vehicleObj->getPos()[2] + 10.0);
	viewMatrix = glm::lookAt(cameraPos, vehicleObj->getPos(), up);
	pitch = 7;
	distanceFromVehicle = 10;
}

// Set the mode of the camera
void Camera::setCameraMode(int mode) {
	cameraMode = mode;
}

void Camera::update() {
	float horizontalDistance = calcHorizontalDistance();
	float verticalDistance = calcVerticalDistance();
	calcCameraPosition(horizontalDistance, verticalDistance);

	at = vehicleObj->getPos();

	if (cameraMode == FINISH_CAM) {
		calcCameraPosition(1, verticalDistance);
	}

	viewMatrix = glm::lookAt(cameraPos, at, up);
}

glm::mat4 Camera::getViewMatrix() {
	return viewMatrix;
}

// move camera based on current position/camera type
void Camera::calcCameraPosition(float horDistance, float vertDistance){
	float theta = vehicleObj->getRotation();
	float offsetX = (float) (horDistance * sin(DEG2RAD(theta)));
	float offsetZ = (float) (horDistance * cos(DEG2RAD(theta)));
	
	if (cameraMode == THIRD_PERSON) {
		cameraPos[0] = vehicleObj->getPos()[0] + offsetX;
		cameraPos[2] = vehicleObj->getPos()[2] + offsetZ;
		cameraPos[1] = vehicleObj->getPos()[1] - vertDistance + 7;
	}
	else if(cameraMode == FINISH_CAM){
		cameraPos[0] = vehicleObj->getPos()[0] + offsetX - 10;
		cameraPos[2] = vehicleObj->getPos()[2] + offsetZ;
		cameraPos[1] = vehicleObj->getPos()[1] - vertDistance + 3;
	}
}

float Camera::calcHorizontalDistance(){
	return (float) (distanceFromVehicle * cos(DEG2RAD(pitch+4)));
}

float Camera::calcVerticalDistance(){
	return (float) (distanceFromVehicle * sin(DEG2RAD(pitch+4)));
}