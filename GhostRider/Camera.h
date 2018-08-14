#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Vehicle.h"

class Camera {
	private:
		int cameraMode;
		glm::mat4 viewMatrix;
		glm::vec3 up, at, cameraPos;
		Vehicle *vehicleObj;

		float pitch, distanceFromVehicle;

	public:
		Camera(Vehicle *v);
		void setCameraMode(int mode);
		void update();
		glm::mat4 getViewMatrix();
		void calcCameraPosition(float horDistance, float vertDistance);
		float calcHorizontalDistance();
		float calcVerticalDistance();
};

#endif