#ifndef MAINMENU_H
#define MAINMENU_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "external_files/stb_image.h"

class MainMenu {
	private:
		unsigned int PID;
		unsigned int vaoHandle;
		unsigned int texID;
		int createVAO(unsigned int PID);
		void createTextures(unsigned int PID);
	public:
		MainMenu(unsigned int programID);
		void render(unsigned int PID);
};

#endif