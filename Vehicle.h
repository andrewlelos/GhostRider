#ifndef VEHICLE_H
#define VEHICLE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <time.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "external_files/InputState.h"
#include "external_files/tiny_obj_loader.h"
#include "external_files/stb_image.h"

class Vehicle {
	private:
		std::vector<std::vector<float> > theVertsVec, theNormVec, theTexVec;
		std::vector<std::vector<unsigned int> > theIndsVec;

		unsigned int *vaoHandle, *texID;
		unsigned int PID;

		int numShapes, numMaterials, numTextures;

		glm::mat4 model;
		glm::vec3 position;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		float rotation;
		int chosenVehicle;

		int createTextures();
		void loadRGBTexture(const char *path);
		void createVAO(unsigned int PID);
		void checkBounds();

		int turn;

		std::string basePath;

		bool gameover;
		
	public:
		Vehicle(unsigned int programID);
		~Vehicle();
		void render(unsigned int PID);
		glm::vec3 getPos();
		void update(double dt, GLFWwindow *window);
		float getRotation();
		void loadOBJ(int drawMode);
		glm::mat4 createTransformationMatrix(glm::vec3 translation, float rotation);
		void checkInput(GLFWwindow *window);
		void gameOver();
};

#endif