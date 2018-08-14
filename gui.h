#ifndef GUI_H
#define GUI_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "external_files/stb_image.h"
#include "Tetra.h"

class Gui
{

private:
    unsigned int vaoHandle;
    unsigned int id;
    GLuint* guiTex;
    bool gameover;
    Tetra* tetra;
    int exitTime, offsetTime;
    
public:
    Gui(unsigned int programid, Tetra *t);
    ~Gui();
    int createGuiVAO();
    void render(unsigned int programID);
    void loadRGBTexture(const char *path);
    std::vector<int> returnTime(int time);
    void generateTextures();
    void gameOver();
    void setOffsetTime(int offset);
};
#endif