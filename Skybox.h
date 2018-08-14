#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <iostream>
#include <string>

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "external_files/stb_image.h"

class Skybox
{

private:
    unsigned int vaoHandle;
    unsigned int id;
    GLuint skyboxTex;
    
public:
    Skybox(unsigned int programid, float size, std::string texFolder, std::string ext);
    int createCubeVAO(float size);
    void render(unsigned int programID, glm::mat4 viewMtx);
    void loadRGBATexture(const char *path, int i);
    void createTextures(std::string texFolder, std::string ext);
};
#endif