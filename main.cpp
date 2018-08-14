/* Ghost Rider
    Written by Andrew Lelos & Lockie Richter
*/

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/* All classes */
#include "external_files/Shader.hpp"
#include "external_files/InputState.h"
#include "racingTrack.h"
#include "Vehicle.h"
#include "Camera.h"
#include "Lamp.h"
#include "Barrel.h"
#include "MainMenu.h"
#include "gui.h"
#include "Skybox.h"
#include "Tetra.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external_files/stb_image.h"

#define THIRD_PERSON 0
#define FINISH_CAM 1

#define SCOOTER 1
#define CAR 2
#define QUAD 3
#define POLICE_BIKE 4

/* shader ID's */
unsigned int mapShaderID, VehicleShaderID, menuShaderID, guiShaderID, skyboxShaderID, tetraShaderID;

/* lighting variables */
unsigned int street_lightambientHandle, street_lightdiffuseHandle, street_lightspecularHandle;
unsigned int lampPosHandle;
float lampPos[] = {-2.0, 0.0, 5.0};

double dt, now, lastCall;
int totalScore = 0;
bool isClicked, offsetTimeSet;
int winX = 750, winY = 750;

/* objects */
GLFWwindow* window;
racingTrack *theTrack;
Vehicle *vehicle;
Camera *theCamera;
Lamp *post;
Barrel *barrel;
MainMenu *menu;
Gui *gui;
Skybox *skybox;
Tetra *tetra;

InputState Input;

/* camera variables */
glm::vec3 cameraPos;
glm::vec3 zero(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);
glm::vec3 pos;
glm::mat4 viewMtx;
int cameraMode;

/* Set the lighting conditions for the street lamp */
void setLampLight() {
    glUseProgram(mapShaderID);
    float lamp_ambient[3] = { 0.1f, 0.1f, 0.1f }; 
    float lamp_diffuse[3] = { 1.0f, 1.0f, 0.0f };
    float lamp_specular[3] = { 0.5f, 0.5f, 0.0f };

    glUniform3fv(street_lightambientHandle, 1, lamp_ambient);
    glUniform3fv(street_lightdiffuseHandle, 1, lamp_diffuse);
    glUniform3fv(street_lightspecularHandle, 1, lamp_specular);

    float lightPos[3];

    // The light needs to be at the top of the lamp.
    lightPos[0] = lampPos[0];
    lightPos[1] = lampPos[1] + 7;
    lightPos[2] = lampPos[2]; 

    glUniform3fv(lampPosHandle, 1, lightPos);
}

/* Set the lamp lighting uniform variables. */
int setupShader(unsigned int id) {
    glUseProgram(id);

    lampPosHandle = glGetUniformLocation(id, "lampPos");

    street_lightambientHandle = glGetUniformLocation(id, "street_light_ambient");
    street_lightdiffuseHandle = glGetUniformLocation(id, "street_light_diffuse");
    street_lightspecularHandle = glGetUniformLocation(id, "street_light_specular");
    if ( street_lightambientHandle == -1 ||
         street_lightdiffuseHandle == -1 ||
         street_lightspecularHandle == -1) {
        fprintf(stderr, "Error: can't find bike light uniform variables\n");
        return 1;
    } 

    return 0;   // return success
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_ESCAPE: // escape key pressed
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;    
            default:
                break;
        }
    }
}

void setProjection() {
    glm::mat4 projection = glm::perspective( (float)M_PI/3.0f, (float) winX / winY, 1.0f, 100.0f );

    /* to make the game look as if its in the sky, 
         the skybox has a z projection of 0 */
    glm::mat4 skyboxProjection = glm::perspective( (float)M_PI/3.0f, (float) winX / winY, 1.0f, 0.0f );

    /* Load it to the shader programs */
    int projHandleMap = glGetUniformLocation(mapShaderID, "projection");
    int projHandleVehicle = glGetUniformLocation(VehicleShaderID, "projection");
    int projHandleMenu = glGetUniformLocation(menuShaderID, "projection");
    int projHandleSkybox = glGetUniformLocation(skyboxShaderID, "projection");
    int projHandleTetra = glGetUniformLocation(tetraShaderID, "projection");

    if (projHandleMap == -1 || projHandleVehicle == -1 || projHandleMenu == -1 || projHandleSkybox == -1 || projHandleTetra == -1) {
        std::cout << "Uniform: projection is not an active uniform label\n";
    }
    glUseProgram(mapShaderID);
    glUniformMatrix4fv( projHandleMap, 1, false, glm::value_ptr(projection) );

    glUseProgram(VehicleShaderID);
    glUniformMatrix4fv( projHandleVehicle, 1, false, glm::value_ptr(projection) );

    glUseProgram(menuShaderID);
    glUniformMatrix4fv(projHandleMenu, 1, false, glm::value_ptr(projection));

    glUseProgram(skyboxShaderID);
    glUniformMatrix4fv(projHandleSkybox, 1, false, glm::value_ptr(skyboxProjection));

    glUseProgram(tetraShaderID);
    glUniformMatrix4fv(projHandleTetra, 1, false, glm::value_ptr(projection));
} 

// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y )  {
    winX = x;
    winY = y;
    setProjection();
    glViewport( 0, 0, x, y );
}

void mouse_pos_callback(GLFWwindow* window, double x, double y) {
    Input.update((float)x, (float)y);
}    

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        Input.rMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        Input.rMousePressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        Input.lMousePressed = true;
        /*  If the menu hasn't been clicked yet,
            select the model and load it to the GPU
         */
        if (!isClicked) {
            if (Input.prevX >= 0.5 * winX && Input.prevY >= 0.5 * winY) {
                vehicle->loadOBJ(CAR);
            } else if (Input.prevX <= 0.5 * winX && Input.prevY >= 0.5 * winY) {
                vehicle->loadOBJ(SCOOTER);
            } else if (Input.prevX <= 0.5 * winX && Input.prevY <= 0.5 * winY) {
                vehicle->loadOBJ(POLICE_BIKE);
            } else if (Input.prevX >= 0.5 * winX && Input.prevY <= 0.5 * winY) {
                vehicle->loadOBJ(QUAD);
            }
            isClicked = true;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Input.lMousePressed = false;
    }                
}

void render() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /*  If the vehicle hasn't beeen selected only
        draw the start up menu. Otherwise draw the scene as 
        normal.
     */
    if (!isClicked) {
        viewMtx = glm::lookAt(glm::vec3(0.0, 0.0, -20.0), zero, up);
        int viewHandleMenu = glGetUniformLocation(menuShaderID, "view");
        glUseProgram(menuShaderID);
        glUniformMatrix4fv(viewHandleMenu, 1, false, glm::value_ptr(viewMtx));
        menu->render(menuShaderID);
    } else {
        /* ensure timer starts at 60,
            no matter how long user spends on menu */
        if(!offsetTimeSet){
            int offsetTime = glfwGetTime();
            gui->setOffsetTime(offsetTime);
            offsetTimeSet = true;
        }

        int viewHandleMap = glGetUniformLocation(mapShaderID, "view");
        int viewHandleVehicle = glGetUniformLocation(VehicleShaderID, "view");
        int viewHandleTetra = glGetUniformLocation(tetraShaderID, "view");
        if (viewHandleMap == -1 || viewHandleVehicle == -1 || viewHandleTetra == -1) {
            std::cout << "Uniform: view is not an active uniform label\n";
        }
        glUseProgram(mapShaderID);
        glUniformMatrix4fv( viewHandleMap, 1, false, glm::value_ptr(viewMtx) );
        glUseProgram(VehicleShaderID);
        glUniformMatrix4fv( viewHandleVehicle, 1, false, glm::value_ptr(viewMtx) );
        glUseProgram(tetraShaderID);
        glUniformMatrix4fv( viewHandleTetra, 1, false, glm::value_ptr(viewMtx) );


        /* update all objects that require delta time */
        now = glfwGetTime();
        dt = now - lastCall;
        vehicle->update(dt, window);
        tetra->update(dt, vehicle->getPos());
        lastCall = glfwGetTime();
        theCamera->update();
        viewMtx = theCamera->getViewMatrix();


        /* trigger game over mode if timer hits 0 */
        std::vector<int> currTime = gui->returnTime(glfwGetTime());
        if(currTime[0] == 0 && currTime[1] == 0){
            theCamera->setCameraMode(FINISH_CAM);
            gui->gameOver();
            vehicle->gameOver();
            tetra->gameOver();
        }

        /* render all objects */
        skybox->render(skyboxShaderID, viewMtx);
        theTrack->render(mapShaderID);
        post->render(mapShaderID);
        vehicle->render(VehicleShaderID);
        barrel->render(mapShaderID);
        gui->render(guiShaderID);
        tetra->render(tetraShaderID);
    }
}

/**
 * Error callback for GLFW. Simply prints error message to stderr.
 */
static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

int main (int argc, char **argv) {
    isClicked = false;
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        fprintf(stderr, "Couldn't initialise GLFW\n");
        exit(1);
    }

    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window and OpenGL context
    window = glfwCreateWindow(winX, winY, "Ghost Rider!", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "Failed to initialise window\n");
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(1);
    }

    // Load up the shaders we are to use. 0 indicates error.
    mapShaderID = LoadShaders("mapShader.vert", "mapShader.frag");
    VehicleShaderID = LoadShaders("vehicleShader.vert", "vehicleShader.frag");
    menuShaderID = LoadShaders("menu.vert", "menu.frag");
    guiShaderID = LoadShaders("gui.vert", "gui.frag");
    skyboxShaderID = LoadShaders("Skybox.vert", "Skybox.frag");
    tetraShaderID = LoadShaders("Tetra.vert", "Tetra.frag");
    if (mapShaderID == 0) {
        printf("Failed to load map shaders\n");
        exit(1);
    }
    if (VehicleShaderID == 0) {
        printf("Failed to load Vehicle shaders\n");
        exit(1);
    }
    if (menuShaderID == 0) {
        printf("Failed to load menu shaders\n");
        exit(1);
    }
    if (menuShaderID == 0) {
        printf("Failed to load gui shaders\n");
        exit(1);
    }
    if (skyboxShaderID == 0) {
        printf("Failed to load skybox shaders\n");
        exit(1);
    }
    if (tetraShaderID == 0) {
        printf("Failed to load tetra shaders\n");
        exit(1);
    }

    // Set OpenGL state we need for this application.
    glClearColor(0.0F, 0.75F, 1.0F, 0.0F);
    glEnable(GL_DEPTH_TEST);
    
    // Set up the scene and the cameras
    setProjection();

    // Define callback functions and start main loop
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);

    /* create all objects */
    skybox = new Skybox(skyboxShaderID, 100.0f, "clouds/", ".png");
    theTrack = new racingTrack(mapShaderID);
    vehicle = new Vehicle(VehicleShaderID);
    pos = vehicle->getPos();
    theCamera = new Camera(vehicle);
    viewMtx = theCamera->getViewMatrix();
    cameraMode = THIRD_PERSON;
    post = new Lamp(mapShaderID, lampPos);
    barrel = new Barrel(mapShaderID);
    menu = new MainMenu(menuShaderID);
    tetra = new Tetra(tetraShaderID);
    gui = new Gui(guiShaderID, tetra);

    setupShader(mapShaderID);
    setLampLight();

    while (!glfwWindowShouldClose(window)) {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    delete theTrack;
    if(isClicked){
        delete vehicle;
    }
    delete post;
    delete barrel;
    delete menu;
    delete gui;
    delete skybox;

    return 0;
}