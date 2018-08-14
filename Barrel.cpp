/* Written by Lockie Richter - a1628315 */

#include "Barrel.h"

#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define VALS_PER_COLOUR 3
#define VALS_PER_TEX 2

Barrel::Barrel(unsigned int programID) {
	this->PID = programID;
	position = glm::vec3(0.0, 0.3, 3.5);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(20.0, 20.0, 20.0));
	
	std::string objectPath = "external_files/Barrel/Barrel02.obj";

	basePath = objectPath.substr(0, objectPath.find_last_of("/")+1);

	std::string error;

	bool result = tinyobj::LoadObj(shapes, materials, error, 
		objectPath.c_str(), basePath.c_str(), true);
	if (error.length() != 0) {
		fprintf(stderr, "%s\n", error.c_str());
		exit(1);
	}

	numShapes = shapes.size();
	numMaterials = numShapes;

	std::vector<tinyobj::material_t> tempMaterials;
	for (int i=0; i<numMaterials; i++) {
		int index = shapes[i].mesh.material_ids[0];
		tempMaterials.push_back(materials[index]);
	}
	materials = tempMaterials;

	for (int i=0; i<numShapes; i++) {
		theVertsVec.push_back(shapes[i].mesh.positions);
		theNormVec.push_back(shapes[i].mesh.normals);
		theIndsVec.push_back(shapes[i].mesh.indices);
		theTexVec.push_back(shapes[i].mesh.texcoords);
	}

	for (int i=0; i<theNormVec.size(); i++) {
		if (theNormVec[i].empty()) {
			for (int j=0; j<theVertsVec[i].size(); j++) {
				theNormVec[i][j] = 0.0f;
			}
		}
	}

	// Count the number of textures.
	for (int i=0; i<numMaterials; i++) {
		if (!materials[i].diffuse_texname.empty()) {
			numTextures++;
		}
	}

	// Crate the VAO for each shader program.
	createVAO(PID);

	if (createTextures() != 0) {
		fprintf(stderr, "Error creating textures.\n");
		exit(1);
	}
}

Barrel::~Barrel() {
	delete vaoHandle;
	delete texID;
}

/*	Create a VAO and bind it to the shader program
	that is PID.
 */
void Barrel::createVAO(unsigned int PID) {
	glUseProgram(PID);
	vaoHandle = new unsigned int[numShapes];

	int vertextLocation = glGetAttribLocation(PID, "a_vertex");
	int normalLocation = glGetAttribLocation(PID, "a_normal");
	int textureLocation = glGetAttribLocation(PID, "a_texcoord");

	if (textureLocation == -1) {
		fprintf(stderr, "Error getting texture location!\n");
		exit(1);
	}

	/*	Each shape will be bound once to the GPU. Convert
		the vectors to arrays and send this data to the GPU.
	 */
	for (int i=0; i<numShapes; i++) {
		float verticesArray[theVertsVec[i].size()];
		float normalsArray[theNormVec[i].size()];
		float texCoordsArray[theTexVec[i].size()];
		unsigned int indicesArray[theIndsVec[i].size()];

		std::copy(theVertsVec[i].begin(), theVertsVec[i].end(), verticesArray);
		std::copy(theNormVec[i].begin(), theNormVec[i].end(), normalsArray);
		std::copy(theIndsVec[i].begin(), theIndsVec[i].end(), indicesArray);
		std::copy(theTexVec[i].begin(), theTexVec[i].end(), texCoordsArray);

		glGenVertexArrays(1, &vaoHandle[i]);
		glBindVertexArray(vaoHandle[i]);

		unsigned int buffer[4];
		glGenBuffers(4, buffer);

			// Set vertex position attribute
		glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(float) * theVertsVec[i].size(),
			verticesArray,
			GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertextLocation);
		glVertexAttribPointer(vertextLocation, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

			// Normal attributes
		glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(float) * theNormVec[i].size(),
			normalsArray,
			GL_STATIC_DRAW);
		glEnableVertexAttribArray(normalLocation);
		glVertexAttribPointer(normalLocation, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* theTexVec[i].size(),
			texCoordsArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(textureLocation);
		glVertexAttribPointer(textureLocation, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

        // Vertex indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned int) * theIndsVec[i].size(),
			indicesArray,
			GL_STATIC_DRAW);
	}
}

void Barrel::render(unsigned int PID) {
	glUseProgram(PID);
	int normHandle = glGetUniformLocation(PID, "normal_matrix");
	int modelUniformHandle = glGetUniformLocation(PID, "model");
	if (normHandle == -1) {
		printf("Error: can't find model matrix uniforms\n");
		exit(1);
	}

	// Draw each shape once.
	for (int i=0; i<numShapes; i++) {
		glBindVertexArray(vaoHandle[i]);

		glm::mat3 normMatrix;
		normMatrix = glm::mat3(model);
		glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
		glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
		
		int textureHandle = glGetUniformLocation(PID, "texMap");
		if (textureHandle == -1) {
			fprintf(stderr, "Error: can't find texture maps\n");
			exit(1);
		}
		
		// Bind the current texture and draw the elements.
		glBindTexture( GL_TEXTURE_2D, texID[i] );
		glDrawElements(GL_TRIANGLES, theIndsVec[i].size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glFlush();
	}
}

// Load a texture onto the GPU
void Barrel::loadRGBTexture(const char *path) {
	int x, y, n;

    // Load from file. Force RGB image pixel format
	unsigned char *data = stbi_load(path, &x, &y, &n, 3);

    // Copy to GPU as data for the currently active texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	stbi_image_free(data);
	
	glGenerateMipmap(GL_TEXTURE_2D);
}

// Create a texture.
int Barrel::createTextures() {
	texID = new unsigned int[numMaterials];
	glGenTextures(numMaterials, texID );
	glActiveTexture(GL_TEXTURE0);
	for (int i=0; i<numMaterials; i++) {
		glBindTexture( GL_TEXTURE_2D, texID[i] );
		std::string texture = basePath + materials[i].diffuse_texname;
		loadRGBTexture(texture.c_str());
	}
	return 0;
}