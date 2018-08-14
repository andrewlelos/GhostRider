#include "Skybox.h"

Skybox::Skybox(unsigned int programid, float size, std::string texFolder, std::string ext){
    id = programid;
	vaoHandle = createCubeVAO(size);
    createTextures(texFolder, ext);
}

int Skybox::createCubeVAO(float size)
{
    float points[] = {
  		-size,  size, -size,
  		-size, -size, -size,
   		size, -size, -size,
   		size, -size, -size,
   		size,  size, -size,
  		-size,  size, -size,
  
  		-size, -size,  size,
  		-size, -size, -size,
  		-size,  size, -size,
  		-size,  size, -size,
  		-size,  size,  size,
  		-size, -size,  size,
  
   		size, -size, -size,
   		size, -size,  size,
   		size,  size,  size,
   		size,  size,  size,
   		size,  size, -size,
   		size, -size, -size,
   
  		-size, -size,  size,
  		-size,  size,  size,
   		size,  size,  size,
   		size,  size,  size,
   		size, -size,  size,
  		-size, -size,  size,
  
  		-size,  size, -size,
   		size,  size, -size,
   		size,  size,  size,
   		size,  size,  size,
  		-size,  size,  size,
  		-size,  size, -size,
  
  		-size, -size, -size,
  		-size, -size,  size,
   		size, -size, -size,
   		size, -size, -size,
  		-size, -size,  size,
   		size, -size,  size
	};

	glUseProgram(id);
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	unsigned int buffer;
	glGenBuffers(1, &buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
                 6 * 18 * sizeof(float), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    
	return vaoHandle;
}

void Skybox::render(unsigned int programID, glm::mat4 viewMtx){
	glUseProgram(programID);
	glBindVertexArray(vaoHandle);
	glEnableVertexAttribArray(0);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	int viewUniformHandle = glGetUniformLocation(programID, "view");

    if (viewUniformHandle == -1) {
        std::cout << "Uniform: view is not an active uniform label\n";
    }
    glUniformMatrix4fv( viewUniformHandle, 1, false, glm::value_ptr(viewMtx) );

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	glFlush();
}

void Skybox::createTextures(std::string texFolder, std::string ext){
	/* load textures in correct order for cube mapping */
	std::string texOrder[] = {"right", "left", "top", "bottom", "back", "front"};
    glGenTextures(1, &skyboxTex);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    for(int i=0; i<6; i++){
		std::string path = "external_files/skyboxTextures/" + texFolder + texOrder[i] + ext;
        loadRGBATexture(path.c_str(), i);
    }

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

/*
 * Load an image from file and onto the GPU as an RGBA texture.
 */
void Skybox::loadRGBATexture(const char *path, int i)
{
    int x, y, n;

    // Load from file. Force RGB image pixel format
    unsigned char *data = stbi_load(path, &x, &y, &n, STBI_rgb_alpha);

    // Copy to GPU as data for the currently active texture.
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}