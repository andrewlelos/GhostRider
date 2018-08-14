#ifndef TETRA_H
#define TETRA_H

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

class Tetra {
	private:
		std::vector<std::vector<float> > theVertsVec, theNormVec;
		std::vector<std::vector<unsigned int> > theIndsVec;

		unsigned int *vaoHandle;
		unsigned int PID;

		int modelUniformHandle, normHandle;
		int numShapes, numMaterials, totalScore;
		float rotation;

		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		glm::mat4 model;
		glm::vec3 position;

		bool gameover;
		
	public:
		Tetra(unsigned int programID);
		~Tetra();
		void createVAO(unsigned int PID);
		void render(unsigned int PID);
		glm::vec3 getPos();
		void update(double dt, glm::vec3 vehiclePos);
		void loadOBJ();
		void changePosition();
		void drawTetra();
		void checkBounds(glm::vec3 vehiclePos);
		int getScore();
		void gameOver();
};

#endif