#include "gui.h"

Gui::Gui(unsigned int programid, Tetra *t){
    tetra = t;
    id = programid;
    generateTextures();
    vaoHandle = createGuiVAO();
    gameover = false;
    exitTime = 0;
}

Gui::~Gui() {
	delete guiTex;
}

int Gui::createGuiVAO(){
    float points[] = {
        -1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f,

        -1.0f, -1.0f,
        1.0f, -1.0f, 
        1.0f, 1.0f
    };

    glUseProgram(id);
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	unsigned int buffer;
	glGenBuffers(1, &buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(float), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); 

    // Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    
	return vaoHandle;
}

void Gui::render(unsigned int programID){
    glUseProgram(programID);
    glBindVertexArray(vaoHandle);
    glEnableVertexAttribArray(0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    int modelUniformHandle = glGetUniformLocation(programID, "model");
    int timerNumUniformHandle = glGetUniformLocation(programID, "timerNum");

    int t = glfwGetTime();
    std::vector<int> currTime = returnTime(t);

    /* render while game isn't over
        AND if game is over, render on odd seconds (simulates flashing) */
    if(!gameover || (gameover && t%2 == 1)){
        if(gameover){
            exitTime++;
            if(exitTime == 800){
                exit(1);
            }
        }

        /* first digit of timer */
        glBindTexture(GL_TEXTURE_2D, guiTex[currTime[0]]);
        glm::mat4 model;
        model = glm::translate( model, glm::vec3(0.53, 0.8, 0.0) );
        model = glm::scale( model, glm::vec3( 0.15, 0.15, 0.0) ); 
        glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) ); 
        glDrawArrays(GL_TRIANGLES, 0, 12);

        /* second digit of timer */
        glBindTexture(GL_TEXTURE_2D, guiTex[currTime[1]]);
        glm::mat4 model2;
        model2 = glm::translate( model2, glm::vec3(0.8, 0.8, 0.0) );
        model2 = glm::scale( model2, glm::vec3( 0.15, 0.15, 0.0) ); 
        glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model2) ); 
        glDrawArrays(GL_TRIANGLES, 0, 12);

        /* "SCORE:" */
        glBindTexture(GL_TEXTURE_2D, guiTex[10]);
        glm::mat4 model3;
        model3 = glm::translate( model3, glm::vec3(-0.77, 0.93, 0.0) );
        model3 = glm::scale( model3, glm::vec3( 0.2, 0.05, 0.0) ); 
        glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model3) ); 
        glDrawArrays(GL_TRIANGLES, 0, 12);

        /* first digit of score */
        if(tetra->getScore() < 10){
            glBindTexture(GL_TEXTURE_2D, guiTex[0]);
        }
        else{
            int s1 = tetra->getScore()/10;
            glBindTexture(GL_TEXTURE_2D, guiTex[s1]);
        }
        glm::mat4 model4;
        model4 = glm::translate( model4, glm::vec3(-0.87, 0.77, 0.0) );
        model4 = glm::scale( model4, glm::vec3( 0.09, 0.09, 0.0) ); 
        glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model4) ); 
        glDrawArrays(GL_TRIANGLES, 0, 12);

        /* second digit of score */
        if(tetra->getScore() < 10){
            glBindTexture(GL_TEXTURE_2D, guiTex[tetra->getScore()]);
        }
        else{
            int s2 = tetra->getScore()%10;
            glBindTexture(GL_TEXTURE_2D, guiTex[s2]);
        }
        glm::mat4 model5;
        model5 = glm::translate( model5, glm::vec3(-0.71, 0.77, 0.0) );
        model5 = glm::scale( model5, glm::vec3( 0.09, 0.09, 0.0) ); 
        glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model5) ); 
        glDrawArrays(GL_TRIANGLES, 0, 12);
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
	glFlush();
}

std::vector<int> Gui::returnTime(int time)
{
    std::vector<int> ret;
    if(!gameover){
        int t= (60 + offsetTime) - time;

        int t1 = t/10; // first digit of timer
        int t2 = t%10; // second digit of timer
        ret.push_back(t1);
        ret.push_back(t2);
    }
    else{
        /* timer is 0 if game is over */
        ret.push_back(0);
        ret.push_back(0);
    }
    return ret;
}

void Gui::generateTextures(){
    guiTex = (GLuint*)malloc(sizeof(GLuint) * 11);
    glGenTextures( 11, guiTex );
    glActiveTexture(GL_TEXTURE0);
    for(int i=0; i<=9; i++){
        glBindTexture( GL_TEXTURE_2D, guiTex[i] );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        /*converting i to string for path */
        std::ostringstream stm; 
        stm << i;
        std::string dir = "external_files/bmaps/" + stm.str() + ".png";
        loadRGBTexture( dir.c_str() );
    }

    glBindTexture(GL_TEXTURE_2D, guiTex[10]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    std::string dir = "external_files/bmaps/score.png";
    loadRGBTexture( dir.c_str() );
}

/*
 * Load an image from file and onto the GPU as an RGB texture.
 */
void Gui::loadRGBTexture(const char *path)
{
    int x, y, n;

    // Load from file. Force RGB image pixel format
    unsigned char *data = stbi_load(path, &x, &y, &n, STBI_rgb_alpha);

    // Copy to GPU as data for the currently active texture.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Gui::gameOver(){
    gameover = true;
}

void Gui::setOffsetTime(int offset){
    offsetTime = offset;
}