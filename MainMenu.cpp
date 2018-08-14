#include "MainMenu.h"

#define VALS_PER_VERT 3
#define NUM_VERTS 4
#define NUM_TRIS 2
#define VALS_PER_TEX 2   

MainMenu::MainMenu(unsigned int programID) {
	this->PID = programID;
	vaoHandle = createVAO(PID);
	createTextures(PID);
}

int MainMenu::createVAO(unsigned int PID) {
	glUseProgram(PID);

	int vertLoc = glGetAttribLocation(PID, "a_vertex");
	int texLoc = glGetAttribLocation(PID, "a_texcoord");

	float Vertices[NUM_VERTS * VALS_PER_VERT ] = {
    	-8.0, -8.0, 0.0,
    	8.0, -8.0, 0.0,
    	8.0, 8.0, 0.0,
    	-8.0, 8.0, 0.0
    };

    float texCoords[NUM_VERTS * VALS_PER_TEX] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    unsigned int Indices[NUM_TRIS*3] = {
    	0, 1, 3,
    	1, 2, 3
    };

    glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	unsigned int buffer[3];
	glGenBuffers(3, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(texLoc);
	glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(Indices), Indices, GL_STATIC_DRAW);   

    // Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    
	return vaoHandle;
}

void MainMenu::createTextures(unsigned int PID) {
    glUseProgram(PID);
	glActiveTexture(GL_TEXTURE0);
    glGenTextures( 1, &texID );

	// load an image from file as texture 1
    int x, y, n;
    unsigned char *data = stbi_load("external_files/menuTexture.jpg",
    	&x, &y, &n, 0);

    glBindTexture( GL_TEXTURE_2D, texID );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    
    glGenerateMipmap(GL_TEXTURE_2D); 
}

void MainMenu::render(unsigned int PID) {
	glUseProgram(PID);
    int texHandle = glGetUniformLocation(PID, "texMap");
    if (texHandle == -1) {
    	std::cerr << "Error getting texture uniform" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, texID);
	glBindVertexArray(vaoHandle);

	glDrawElements(GL_TRIANGLES, VALS_PER_VERT * 2, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glFlush();
}