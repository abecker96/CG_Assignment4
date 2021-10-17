//General includes
#include <stdio.h>
#include <iostream>
#include <cstdlib>

//Opengl includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//Project-specific includes
#include "SierpinskiPyramid.h"
#include "IBOCube.h"
#include "AidanGLCamera.h"
#include "UsefulFunctions.h"


// Declaration of Camera object
Camera camera = Camera();

const int numTrees = 100;
const int amountOfSnow = 1000;
// Declaration of Sierpinski Pyramid object(s) as tree leaves
SierpinskiPyramid leaves[numTrees];
// Declaration of tree trunks as cubes
IBOCube trunks[numTrees];
// Ground is a very squished cube
IBOCube ground = IBOCube();
// Moon is also just a cube
IBOCube moon = IBOCube();
// Snow is just a lot of cubes
IBOCube snow[amountOfSnow];

// Global view/projection matrices so they're accessible from
// glfwkeyboard callback functions
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
// A bunch of other globals that are similarly accessible from
// glfwkeyboard callback functions
bool userCameraInput = true;
bool spinningView = false;

// I really need to implement a user interaction method that
// requires fewer global variables
const float cameraSpeed = 1.0f; 
const float mouseSensitivity = 0.05f;                       //Mouse sensitivity
float horizontalAngle = 0.0f;                               //initial camera angle
float verticalAngle = 0.0f;                                 //initial camera angle
float initialFoV = 62.0f;                                   //initial camera field of view
glm::vec3 cameraPosition(0, 0.75, -2);                         //initial camera position

int windowWidth, windowHeight, windowSizeX, windowSizeY;    //Screen space values

// mousebutton callback function
// Performs an action once, the first time a mouse button is pressed
// A left click generates more triangles, while a right click resets to original triangles
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        for(int i = 0; i < numTrees; i++)
        {
            leaves[i].fractalize();
        }
    }
    else if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        for(int i = 0; i < numTrees; i++)
        {
            leaves[i].reset();
        }
    }

}

// keyboard callback function
// Performs an action once, the first time a key is pressed
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{   
    // Check that this event is for an initial key press
    if(action == GLFW_PRESS)
    {
        switch(key){
            case GLFW_KEY_1:        // 1-5 change camera angles
                camera.disableUserInput();
                userCameraInput = false;
                spinningView = false;
                viewMatrix = glm::lookAt(
                    glm::vec3(0.1, 0.75, -1.5),
                    glm::vec3(0, 0.75, 0),
                    glm::vec3(0, 1, 0)
                );
                break;
            case GLFW_KEY_2:
                camera.disableUserInput();
                userCameraInput = false;
                spinningView = false;
                viewMatrix = glm::lookAt(
                    glm::vec3(30, 2, 30),
                    glm::vec3(0, 0, 0),
                    glm::vec3(0, 1, 0)
                );
                break;
            case GLFW_KEY_3:
                camera.disableUserInput();
                userCameraInput = false;
                spinningView = false;
                viewMatrix = glm::lookAt(
                    glm::vec3(0, 30, 0),
                    glm::vec3(0, 0, 0),
                    glm::vec3(1, 0, 0)
                );
                break;
            case GLFW_KEY_4:
                camera.disableUserInput();
                userCameraInput = false;
                spinningView = false;
                viewMatrix = glm::lookAt(
                    glm::vec3(0, 2.5, 0),
                    glm::vec3(0, 0, 0),
                    glm::vec3(1, 0, 0)
                );
                break;
            case GLFW_KEY_5:
                camera.disableUserInput();
                userCameraInput = false;
                spinningView = false;
                viewMatrix = glm::lookAt(
                    glm::vec3(2.5, 1.5, -1.5),
                    glm::vec3(0, 1.5, 0),
                    glm::vec3(0, 1, 0)
                );
                break;
            case GLFW_KEY_6:        // 6 enables an aerial spinning view
                camera.disableUserInput();
                userCameraInput = false;
                spinningView = true;
                break;
            case GLFW_KEY_7:        // 7 restores camera control to user
                camera.enableUserInput();
                userCameraInput = true;
                // re-initialize camera so nothing gets messed up
                camera.init(
                    window, cameraPosition, 
                    glm::perspective(
                        glm::radians<float>(55),
                        (float)windowSizeX/(float)windowSizeY,
                        0.01f, 
                        100.0f
                    ),
                    horizontalAngle, verticalAngle,
                    cameraSpeed*2, mouseSensitivity,
                    true
                );
                break;
            case GLFW_KEY_Q:        // Q toggles wireframe rendering
                for(int i = 0; i < numTrees; i++)
                {
                    trunks[i].drawAsWireframe();
                    leaves[i].drawAsWireframe();
                }
                for(int i = 0; i < amountOfSnow; i++)
                {
                    snow[i].drawAsWireframe();
                }
                ground.drawAsWireframe();
                moon.drawAsWireframe();
                break;
            case GLFW_KEY_E:        // E toggles faces rendering
                for(int i = 0; i < numTrees; i++)
                {
                    trunks[i].drawAsFaces();
                    leaves[i].drawAsFaces();
                }
                for(int i = 0; i < amountOfSnow; i++)
                {
                    snow[i].drawAsFaces();
                }
                ground.drawAsFaces();
                moon.drawAsFaces();
                break;
            case GLFW_KEY_R:        // R resets to both wireframe and faces rendering
                for(int i = 0; i < numTrees; i++)
                {
                    trunks[i].drawAsFaces();        // guarantee faces are showing, but no wireframe
                    trunks[i].toggleWireframe();    // turn wireframe back on
                    leaves[i].drawAsFaces();
                    leaves[i].toggleWireframe();
                }
                for(int i = 0; i < amountOfSnow; i++)
                {
                    snow[i].drawAsFaces();
                    snow[i].toggleWireframe();
                }
                ground.drawAsFaces();
                ground.toggleWireframe();
                moon.drawAsFaces();
                moon.toggleWireframe();
                break;
            default:
                break;
        }
    }
}

// Error callback for glfw window problems
// In theory, this should be called automatically
// But I've never seen it run so I don't really know.
void glfwErrorCB(int error, const char* description) {
    fputs(description, stderr);
}


int main() {
    // Start a timer to check frame times
    double start = glfwGetTime();
    double current = start;
    double deltaTime;

    // Necessary due to glew bug
    glewExperimental = true;

    // hook in our GLFW error callback
    glfwSetErrorCallback(glfwErrorCB);

    // Initialize glfw
    if(!glfwInit())
    {
        fprintf( stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Create the window
    glfwWindowHint(GLFW_SAMPLES, 8); // 8x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    // Open a window and create its OpenGL context
    GLFWwindow* window;

    // Check size of screen's available work area
    // This is the area not taken up by taskbars and other OS objects
    glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &windowWidth, &windowHeight, &windowSizeX, &windowSizeY);
    
    // make window
    window = glfwCreateWindow( windowSizeX, windowSizeY, "Aidan Becker Assignment 4", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Ensure we can capture the escape key and mouse clicks being pressed below
    // This sets a flag that a key has been pressed, even if it was between frames
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

    // Initialize camera object
    camera.init(
        window, cameraPosition, 
        glm::perspective(
            glm::radians<float>(55),
            (float)windowSizeX/(float)windowSizeY,
            0.01f, 
            100.0f
        ),
        horizontalAngle, verticalAngle,
        cameraSpeed*2, mouseSensitivity,
        true
    );

    // initialize random number generator for random trees
    srand((int)(glfwGetTime()*100000));

    // set size of the base platform
    float planeSizeX = 15;
    float planeSizeZ = 15;

    leaves[0].init(window, 
        glm::vec3(0, 1, 0),                                     //position in non-modelspace
        glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)),                //scale in non-modelspace
        glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),    //rotation in non-modelspace
        glm::vec3(0, 0.2, 0)                                    //color value
    );
    for(int i = 0; i < 3; i++)
    {
        leaves[0].fractalize();
    }
    trunks[0].init(window,
        glm::vec3(0, 0.3, 0),                         //position in non-modelspace
        glm::scale(glm::vec3(0.3f, 0.7, 0.3f)),                   //scale in non-modelspace
        glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),    //rotation in non-modelspace
        glm::vec3(0.3255, 0.2078, 0.0392)                       //color value
    );
    for(int i = 1; i < numTrees; i++)
    {
        float randX = randomBetween(-1, 1);
        float randZ = randomBetween(-1, 1);
        leaves[i].init(window, 
            glm::vec3(0 + randX*planeSizeX, 1, 0 + randZ*planeSizeZ),   //position in non-modelspace
            glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)),                    //scale in non-modelspace
            glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),        //rotation in non-modelspace
            glm::vec3(0, 0.2, 0)                                        //color value
        );
        // default is a level 3 pyramid
        for(int j = 0; j < 3; j++)
        {
            leaves[i].fractalize();
        }
        trunks[i].init(window,
            glm::vec3(randX*planeSizeX, 0.3, randZ*planeSizeZ),   //position in non-modelspace
            glm::scale(glm::vec3(0.3f, 0.7, 0.3f)),                                   //scale in non-modelspace
            glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),                    //rotation in non-modelspace
            glm::vec3(0.3255, 0.2078, 0.0392)                                       //color value
        );
    }
    for(int i = 0; i < amountOfSnow; i++)
    {
        snow[i].init(window,
            glm::vec3(randomBetween(-planeSizeX,planeSizeX), randomBetween(0, 5), randomBetween(-planeSizeZ, planeSizeZ)),  //position in non-modelspace
            glm::scale(glm::vec3(0.02, 0.02, 0.02)),                                                                        //scale in non-modelspace
            glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),                                                            //rotation in non-modelspace
            glm::vec3(0.9, 0.9, 0.9)                                                                                        //color value
        );  
    }
    ground.init(window,
        glm::vec3(0, 0, 0),                     //position in non-modelspace
        glm::scale(glm::vec3(2*planeSizeX, 0.1, 2*planeSizeZ)),     //scale in non-modelspace
        glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),        //rotation in non-modelspace
        glm::vec3(0.6745, 0.95, 0.6745)                             //color value
    );
    moon.init(window,
        glm::vec3(7, 10, 7),                     //position in non-modelspace
        glm::scale(glm::vec3(1, 1, 1)),     //scale in non-modelspace
        glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),        //rotation in non-modelspace
        glm::vec3(0.678, 0.847, 0.902)        
    );

    // variables for speed of object motion in scene
    float angle = 0.5;
    float snowSpeed = 0.6;
    glm::vec3 tempPosition;

    // Set callback functions for user input
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // Enable depth test so objects render based on closest distance from camera
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set default background color to something closer to a night sky
    glClearColor( 0.0863, 0.106, 0.211, 1.0 );
    do{
        // Update input events
        // Most tutorials do this at the end of the main loop
        // But getting input after frames are calculated might lead to significant 
        // input delay in the event that frames take a while to render
        glfwPollEvents();

        // Clear the screen before drawing new things
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // get time since last frame
        current = glfwGetTime();
        deltaTime = current-start;
        float deltaAngle = angle * deltaTime;
        float snowDistance = snowSpeed*deltaTime;

        // Optionally limit framerate
        // On my computer this does nothing: my framerate is automatically limited to
        // my screen's refresh rate. Need to test if this is the same on windows.
        //if(deltaTime > 0.006944)
        {   
            // Optionally output frametimes
            // std::cout << "New Frame in: " << deltaTime << std::endl;
            // Reset timer
            start = glfwGetTime();
            // Draw!

            // Update the camera's data based on user input
            if(userCameraInput)
            {
                camera.update();
                viewMatrix = camera.getViewMatrix();
                projectionMatrix = camera.getProjectionMatrix();
            }
            // If the spinning view is active, rotate slowly
            else if(spinningView)
            {
                viewMatrix = glm::lookAt(
                    glm::vec3((float)cos(start*0.2)*15, 15, (float)sin(start*0.2)*15),
                    glm::vec3(0, 0, 0),
                    glm::vec3(0, 1, 0)
                );
            }

            for(int i = 0; i < numTrees; i++)
            {
                // rotate every tree that isn't the first one
                if(i != 0){
                    leaves[i].rotate(deltaAngle, glm::vec3(0, 1, 0));
                }
                // draw leaves and trunks
                leaves[i].draw(viewMatrix, projectionMatrix);
                trunks[i].draw(viewMatrix, projectionMatrix);
            }
            for(int i = 0; i < amountOfSnow; i++)
            {
                // make snow fall
                tempPosition = snow[i].getPosition();
                tempPosition = glm::vec3(tempPosition.x, (tempPosition.y-snowDistance), tempPosition.z);
                if(tempPosition.y < 0)
                {
                    tempPosition = tempPosition + glm::vec3(0, 5, 0);
                }
                snow[i].setPosition(tempPosition);
                // draw snow
                snow[i].draw(viewMatrix, projectionMatrix);
            }
            ground.draw(viewMatrix, projectionMatrix);
            moon.draw(viewMatrix, projectionMatrix);

            // actually draw created frame to screen
            glfwSwapBuffers(window);    
        }

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
    
    return 0;
}