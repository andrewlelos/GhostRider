#include "Vehicle.h"

#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define VALS_PER_COLOUR 3
#define VALS_PER_TEX 2

#define PI 3.14159265
#define friction 0.9
#define MAX_SPEED 10.0

#define SCOOTER 1
#define CAR 2
#define QUAD 3
#define POLICE_BIKE 4
#define DEG2RAD(x) float((x)*M_PI/180.0) 

float RUN_SPEED = 15.0f;
float TURN_SPEED = 200.0f;
float currentSpeed = 0.0f;
float currentTurnSpeed = 0.0f;

/*	Set the inital values of everything for the vehicle.
 */
Vehicle::Vehicle(unsigned int programID) {
	this->PID = programID;
	rotation = 0.0f;
	position = glm::vec3(0.0, 0.0, 0.0);
	gameover = false;

	srand(time(NULL));
	turn = 0;
}

Vehicle::~Vehicle() {
	delete vaoHandle;
	delete texID;
}

/*	Load the specific obj file based off of the 
	draw code that is given.
 */
void Vehicle::loadOBJ(int drawMode) {
	std::string scooterPath = "external_files/ScooterModel/Scooter.obj";
	std::string carPath = "external_files/car/car-n.obj";
	std::string quadPath = "external_files/Quad/quad.obj";
	std::string policeBikePath = "external_files/FuturePoliceBike/Future_Police_Bike.obj";
	std::string objectPath = "";

	/* Select the object based on the drawMode, 
		and perform appropriate translation/rotation/scaling */
	if (drawMode == SCOOTER) {
		objectPath = scooterPath;
		chosenVehicle = SCOOTER;
	} else if (drawMode == CAR) {
		objectPath = carPath;
		model = glm::translate(model, glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, (float)PI, glm::vec3(0.0, 1.0, 0.0));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
		chosenVehicle = CAR;
	} else if (drawMode == QUAD) {
		objectPath = quadPath;
		model = glm::scale(model, glm::vec3(0.005, 0.005, 0.005));
		chosenVehicle = QUAD;
	} else if (drawMode == POLICE_BIKE) {
		objectPath = policeBikePath;
		model = glm::rotate(model, (float)PI, glm::vec3(0.0, 1.0, 0.0));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
		chosenVehicle = POLICE_BIKE;
	}

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

/*	Create a VAO and bind it to the shader program
	that is PID.
 */
void Vehicle::createVAO(unsigned int PID) {
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

void Vehicle::render(unsigned int PID) {
	glUseProgram(PID);
	int normHandle = glGetUniformLocation(PID, "normal_matrix");
	int modelUniformHandle = glGetUniformLocation(PID, "model");

	// Draw each shape once.
	for (int i=0; i<numShapes; i++) {
		glBindVertexArray(vaoHandle[i]);

		model = createTransformationMatrix(position, rotation);

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
void Vehicle::loadRGBTexture(const char *path) {
	int x, y, n;

    // Load from file. Force RGB image pixel format
	unsigned char *data = stbi_load(path, &x, &y, &n, 3);

    // Copy to GPU as data for the currently active texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	stbi_image_free(data);
	
	glGenerateMipmap(GL_TEXTURE_2D);
}

// Create a texture.
int Vehicle::createTextures() {
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

/* checks for user input and increases speed/turnSpeed accordingly */
void Vehicle::checkInput(GLFWwindow *window){
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        currentSpeed = -RUN_SPEED;
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        currentSpeed = RUN_SPEED;
    else
        currentSpeed = 0;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        currentTurnSpeed = TURN_SPEED;
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        currentTurnSpeed = -TURN_SPEED;
    else
        currentTurnSpeed = 0;
}

glm::mat4 Vehicle::createTransformationMatrix(glm::vec3 translation, float rotation){
	//create identity
    glm::mat4 modelMtx(1.0f);
    modelMtx = glm::translate(modelMtx, translation);
    modelMtx = glm::rotate(modelMtx, DEG2RAD(rotation), glm::vec3(0.0, 1.0, 0.0));

	if (chosenVehicle == CAR) {
		modelMtx = glm::translate(modelMtx, glm::vec3(0.0, 1.0, 0.0));
		modelMtx = glm::rotate(modelMtx, (float)PI, glm::vec3(0.0, 1.0, 0.0));
		modelMtx = glm::scale(modelMtx, glm::vec3(0.5, 0.5, 0.5));
	} else if (chosenVehicle == QUAD) {
		modelMtx = glm::scale(modelMtx, glm::vec3(0.005, 0.005, 0.005));
	} else if (chosenVehicle == POLICE_BIKE) {
		modelMtx = glm::rotate(modelMtx, (float)PI, glm::vec3(0.0, 1.0, 0.0));
		modelMtx = glm::scale(modelMtx, glm::vec3(0.5, 0.5, 0.5));
	}

    return modelMtx;
}

/* move vehicle correctly based on delta time */
void Vehicle::update(double dt, GLFWwindow *window) {	
	if(!gameover){
		checkInput(window);
	}
	rotation += currentTurnSpeed * float(dt);
	float distance = currentSpeed * float(dt);
	float dx = (float) distance * sin(rotation*(M_PI/180.0f));
	float dz = (float) distance * cos(rotation*(M_PI/180.0f));
	position[0] += dx;
	position[2] += dz;
	
  checkBounds();   
}

/* check for collisions with map/barrel */
void Vehicle::checkBounds() {
	for (int i=0; i<3; i++) {
		// Check to see if inside the map
		if (position[i] > 19.2) {
			if(!gameover){
				system("afplay external_files/explosion.wav &"); // play sound on mac
				//system("play external_files/explosion.wav &"); // play sound on linux
			}
			position[i] = 19.0f;
		} else if (position[i] < -19.2) {
			if(!gameover){
				system("afplay external_files/explosion.wav &"); // play sound on mac
				//system("play external_files/explosion.wav &"); // play sound on linux
			}
			position[i] = -19.0f;
		}
	}

	// Check to see if collision with Barrel
	if (position[0] < 0.3 && position[0] > -0.3 && position[2] > 2.2 && position[2] < 4.85) {
		if(position[2] > 3){
			position[2] = 4.85;
		}
		else{
			position[2] = 2.2;
		}
	}
}

glm::vec3 Vehicle::getPos() {
	return position;
}

float Vehicle::getRotation() {
	return rotation;
}

/* when game is over, vehicle drives on its own */
void Vehicle::gameOver(){
	gameover = true;
	currentSpeed = -RUN_SPEED;

	turn++;
	turn %= 300;

	if(turn <= 140){
		currentTurnSpeed = TURN_SPEED;
	}
	else if (turn >= 200){
		currentTurnSpeed = -TURN_SPEED;
	}
	else{
		currentTurnSpeed = 0;
	}
}