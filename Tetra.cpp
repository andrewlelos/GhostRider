#include "Tetra.h"

#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define VALS_PER_COLOUR 3
#define VALS_PER_TEX 2

/*	Set the inital values of everything for the vehicle.
 */
Tetra::Tetra(unsigned int programID) {
	this->PID = programID;
	position = glm::vec3(0.0, 0.7, -5.5);
	model = glm::translate(model, position);
	loadOBJ();
	totalScore = 0;
	gameover = false;
	srand(time(NULL));
}

Tetra::~Tetra() {
	delete vaoHandle;
}

/*	Load the specific obj file based off of the 
	draw code that is given.
 */
void Tetra::loadOBJ() {
	std::string objectPath = "external_files/tetra/tetra.obj";
	std::string basePath = objectPath.substr(0, objectPath.find_last_of("/")+1);
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
	}

	for (int i=0; i<theNormVec.size(); i++) {
		if (theNormVec[i].empty()) {
			for (int j=0; j<theVertsVec[i].size(); j++) {
				theNormVec[i][j] = 0.0f;
			}
		}
	}

	// Crate the VAO for each shader program.
	createVAO(PID);

}

/*	Create a VAO and bind it to the shader program
	that is PID.
 */
void Tetra::createVAO(unsigned int PID) {
	glUseProgram(PID);
	vaoHandle = new unsigned int[numShapes];

	int vertextLocation = glGetAttribLocation(PID, "a_vertex");
	int normalLocation = glGetAttribLocation(PID, "a_normal");

	/*	Each shape will be bound once to the GPU. Convert
		the vectors to arrays and send this data to the GPU.
	 */
	for (int i=0; i<numShapes; i++) {
		float verticesArray[theVertsVec[i].size()];
		float normalsArray[theNormVec[i].size()];
		unsigned int indicesArray[theIndsVec[i].size()];

		std::copy(theVertsVec[i].begin(), theVertsVec[i].end(), verticesArray);
		std::copy(theNormVec[i].begin(), theNormVec[i].end(), normalsArray);
		std::copy(theIndsVec[i].begin(), theIndsVec[i].end(), indicesArray);

		glGenVertexArrays(1, &vaoHandle[i]);
		glBindVertexArray(vaoHandle[i]);

		unsigned int buffer[3];
		glGenBuffers(3, buffer);

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

        // Vertex indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned int) * theIndsVec[i].size(),
			indicesArray,
			GL_STATIC_DRAW);
	}
}

void Tetra::render(unsigned int PID) {
	glUseProgram(PID);
	normHandle = glGetUniformLocation(PID, "normal_matrix");
	modelUniformHandle = glGetUniformLocation(PID, "model");

	if (normHandle == -1) {
		printf("Error: can't find model matrix uniforms\n");
		exit(1);
	}

	drawTetra();
}

void Tetra::drawTetra(){
	// Draw each shape once.
	for (int i=0; i<numShapes; i++) {
		glBindVertexArray(vaoHandle[i]);

		glm::mat3 normMatrix;
		normMatrix = glm::mat3(model);
		glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
		glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));

		glDrawElements(GL_TRIANGLES, theIndsVec[i].size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glFlush();
	}
}


void Tetra::update(double dt, glm::vec3 vehiclePos) {
	/* only allow points to be gained before game is over */
	if(!gameover){
		checkBounds(vehiclePos);
	}

	/* rotate the tetra at a constant speed based on delta time */
	model = glm::mat4(1);	
	rotation += 2.5 * float(dt);
	model = glm::translate(model, position);
	model = glm::rotate(model, rotation, glm::vec3(0.0, 1.0, 0.0));
}

void Tetra::checkBounds(glm::vec3 vehiclePos){
	/* if vehicle passes through tetra,
		increase score and change position of tetra */
	if(vehiclePos[2] <= position[2]+1.2 && vehiclePos[2] >= position[2]-1.2){
            if(vehiclePos[0] <= position[0]+1.2 && vehiclePos[0] >= position[0]-1.2){
                changePosition();
                totalScore++;
            }
        }
}

glm::vec3 Tetra::getPos(){
	return position;
}

void Tetra::changePosition(){
	system("afplay external_files/ding.wav &"); //command to play sound for mac
	//system("play external_files/ding.wav &"); //command to play sound for linux

	/* find random number between 1-39 for new position */
	int x = rand() % 38 + 1;
	int z = rand() % 38 + 1;

	/* if >= 20, make negative as to cover all of the map */
	if(x >=20){
		x -= 39;
	}
	if(z >= 20){
		z -= 39;
	}
	position[0] = x;
	position[2] = z;
}

int Tetra::getScore(){
	return totalScore;
}

void Tetra::gameOver(){
	gameover = true;
}