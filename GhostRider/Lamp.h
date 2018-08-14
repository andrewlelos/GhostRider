#ifndef LAMP_H
#define LAMP_H

#include <iostream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "external_files/InputState.h"
#include "external_files/tiny_obj_loader.h"
#include "external_files/stb_image.h"

class Lamp {
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

		float xPos, yPos, zPos;

		int createTextures();
		void loadRGBTexture(const char *path);
		void createVAO(unsigned int PID);
		std::string basePath;
		
	public:
		Lamp(unsigned int programID, float pos[3]);
		~Lamp();
		void render(unsigned int PID);
};

#endif