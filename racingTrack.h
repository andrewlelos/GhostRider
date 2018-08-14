#ifndef RACINGTRACK_H
#define RACINGTRACK_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "external_files/stb_image.h"

class racingTrack {
	private:
		unsigned int vaoHandle;
		unsigned int texID;
		glm::mat4 model;
		
		int createVAO(unsigned int PID);
		void createTextures(unsigned int PID);

	public:
		racingTrack(unsigned int PID);
		void render(unsigned int PID);
};

#endif