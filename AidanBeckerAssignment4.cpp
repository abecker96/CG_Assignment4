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

const int numTrees = 150;
// Declaration of Sierpinski Pyramid object(s) as tree leaves
SierpinskiPyramid leaves[numTrees];
// Declaration of tree trunks as cubes
IBOCube trunks[numTrees];
// Ground is a very squished cube
IBOCube ground = IBOCube();

// mousebutton callback function
// A left click generates more triangles, while a right click resets to a new triangle
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{   
    // Check that this event is for an initial key press
    if(action == GLFW_PRESS)
    {
        switch(key){
            case GLFW_KEY_1:        // 1-4 change colors
                
                break;
            case GLFW_KEY_2:
                
                break;
            case GLFW_KEY_3:
                
                break;
            case GLFW_KEY_4:
                
                break;
            case GLFW_KEY_Q:        // Q toggles wireframe rendering
                
                break;
            case GLFW_KEY_E:        // E toggles faces rendering
                
                break;
            case GLFW_KEY_T:

                break;
            case GLFW_KEY_R:

                break;
            default:
                break;
        }
    }
}

// Error callback for glfw window problems
void glfwErrorCB(int error, const char* description) {
    fputs(description, stderr);
}


int main() {
    int windowWidth, windowHeight, windowSizeX, windowSizeY;    //Screen space values

    const float cameraSpeed = 3.0f;
    const float mouseSensitivity = 0.05f;                       //Mouse sensitivity
    float horizontalAngle = 0.0f;                               //initial camera angle
    float verticalAngle = 0.0f;                                 //initial camera angle
    float initialFoV = 62.0f;                                   //initial camera field of view
    glm::vec3 cameraPosition(0, 1, -2);                         //initial camera position

    // Start a timer to limit framerate and get other information
    double start = glfwGetTime();
    double current = start;
    double deltaTime;

    //Initialize variables for translation and rotation speed
    glm::vec3 translation = glm::vec3(0.01, 0, 0);
    glm::vec3 rotationAxis = glm::vec3(0, 1, 0);
    float rotationSpeed = 0.2;
    float currentRotation = 0;

    // Necessary due to glew bug
    glewExperimental = true;
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
        cameraSpeed*2, mouseSensitivity
    );


    srand((int)(glfwGetTime()*100000));
    float planeSizeX = 20;
    float planeSizeZ = 20;


    leaves[0].init(window, 
        glm::vec3(0, 1, 0),                         //position in non-modelspace
        glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)),    //scale in non-modelspace
        glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),       //rotation in non-modelspace
        glm::vec3(0, 0.2, 0)
    );
    trunks[0].init(window,
        glm::vec3(-0.15, -0.36, -0.15),                         //position in non-modelspace
        glm::scale(glm::vec3(0.3f, 1, 0.3f)),    //scale in non-modelspace
        glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),       //rotation in non-modelspace
        glm::vec3(0.3255, 0.2078, 0.0392)    
    );
    for(int i = 1; i < numTrees; i++)
    {
        float randX = randomBetween(-1, 1);
        float randZ = randomBetween(-1, 1);
        leaves[i].init(window, 
            glm::vec3(0 + randX*planeSizeX, 1, 0 + randZ*planeSizeZ),                         //position in non-modelspace
            glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)),    //scale in non-modelspace
            glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),       //rotation in non-modelspace
            glm::vec3(0, 0.2, 0)
        );
        trunks[i].init(window,
            glm::vec3(-0.15 + randX*planeSizeX, -0.36, -0.15 + randZ*planeSizeZ),                         //position in non-modelspace
            glm::scale(glm::vec3(0.3f, 1, 0.3f)),    //scale in non-modelspace
            glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),       //rotation in non-modelspace
            glm::vec3(0.3255, 0.2078, 0.0392)    
        );
    }
    ground.init(window,
        glm::vec3(-25, 0, -25),
        glm::scale(glm::vec3(50, 0.1, 50)),
        glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)),
        glm::vec3(0, 0.604, 0.0902)
    );


    // Set callback functions for user input
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // Enable depth test so things render based on distance from camera
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    float angle = 0.5;

    // Set default background color to something a little less void-colored
    glClearColor( 0.1, 0.1, 0.1 , 1.0 );
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

        // Optionally limit framerate
        //if(deltaTime > 0.006944)
        {   
            // Optionally output frametimes
            //std::cout << "New Frame in: " << deltaTime << std::endl;
            // Reset timer
            start = glfwGetTime();
            // Draw!

            // Update the camera's data based on user input
            camera.update();
            for(int i = 0; i < numTrees; i++)
            {
                leaves[i].rotate(deltaAngle, glm::vec3(0, 1, 0));
                leaves[i].draw(camera.getViewMatrix(), camera.getProjectionMatrix());
                trunks[i].draw(camera.getViewMatrix(), camera.getProjectionMatrix());
            }
            ground.draw(camera.getViewMatrix(), camera.getProjectionMatrix());
            // sponge.draw(camera.getViewMatrix(), camera.getProjectionMatrix());

            glfwSwapBuffers(window);    // actually draw
        }

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
    

    return 0;
}