

#ifndef SIERPINSKIPYRAMID_H
#define SIERPINSKIPYRAMID_H

//General includes
#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>

//Opengl includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

//Project-specific includes
#include "LoadShaders.h"

// SierpinskiPyramid class
class SierpinskiPyramid {
    public:
        SierpinskiPyramid(){}
        void init(GLFWwindow* window, glm::vec3 position, glm::mat4 scale, glm::mat4 rotation)
        {
            renderFaces = true;
            renderWireframe = true;

            //V0 = 1, -1/sqrt(3), -1/sqrt(6)
            //V1 =  

            // // Load and compile shaders
            tetrahedronShader = LoadShaders("tetrahedronShader.vrt.glsl", "tetrahedronShader.frg.glsl");
            glUseProgram(tetrahedronShader);

            // initialize MVP Matrix array reference in shader
            MVPMatrices_ref = glGetUniformLocation(tetrahedronShader, "matrices");
            if(MVPMatrices_ref < 0)
            {   std::cerr << "couldn't find MVP matrices in shader\n";  }

            // initialize wireframe color reference in shaders
            wireframeColorRef = glGetUniformLocation(tetrahedronShader, "wireframeColor");
            if(wireframeColorRef < 0)
            {   std::cerr<< "couldn't find wireframeColor in shader\n"; }

            // // Set wireframe color
            // glUniform3fv(wireframeColorRef, 1, glm::value_ptr(wireframeColor));

            // setVertData();
            // setColorData();

            // Set up modelMatrix as identity matrix for now
            defaultPosition = position;
            // resetPosition();
            scalingMatrix = scale;
            rotationMatrix = rotation;
            rotateOriginMatrix = glm::mat4(1);
        }
        // draw function
        // Draws every triangle in the vertexbuffer with a color corresponding to the colorbuffer
        void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            // Render relative to the camera
            updateMVPArray(viewMatrix, projectionMatrix);
            glUniformMatrix4fv(MVPMatrices_ref, 6, GL_FALSE, glm::value_ptr(MVPMatrices[0])); // Passing 6 matrices


            // draw triangle faces
            if(renderFaces)
            {
                renderAsFaces();
            }

            // draw triangle wireframe
            if(renderWireframe)
            {
                renderAsWireframe();
            }
        }
        void fractalize()
        {

        }
        void reset(int seed)
        {

        }
        void toggleWireframe()
        {
            renderWireframe = !renderWireframe;
        }
        void toggleFaces()
        {
            renderFaces = !renderFaces;
        }
        void setRotation(float angle, glm::vec3 axis)
        {
            rotationMatrix = glm::rotate(angle, axis);
        }
        void setPosition(const glm::vec3 &position)
        {
            translationMatrix = glm::translate(glm::mat4(1), position);
        }
        void translate(const glm::vec3 &translation)
        {
            translationMatrix = glm::translate(translationMatrix, translation);
        }
        void resetPosition()
        {
            setPosition(defaultPosition);
        }
    private:
        glm::mat4 objectToWorldMatrix, translationMatrix, scalingMatrix, rotationMatrix, rotateOriginMatrix;
        // MVPMatrices will contain the same data as the 5 matrices above
        // The above matrices are just friendly names instead of requiring me to remember index 0 == O2Wmatrix etc
        glm::mat4 MVPMatrices[5];
        GLuint tetrahedronShader;
        GLint MVPMatrices_ref, wireframeColorRef;
        GLFWwindow* window;
        glm::vec3 defaultPosition;
        glm::vec3 wireframeColor = glm::vec3(1.0, 1.0, 1.0);    //Color for the wireframe
        std::vector<glm::vec3> tetrahedronVerts;
        std::vector<glm::vec3> tetrahedronFaces;
        std::vector<glm::vec4> tetrahedrons;
        bool renderFaces, renderWireframe;
        float currentTime;
        // color palettes
        const float colorPalettes[3] = {
            1.0000, 1.0000, 1.0000
        };
        void updateMVPArray(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            // This order is *really* important
            MVPMatrices[0] = scalingMatrix;
            MVPMatrices[1] = rotationMatrix;
            MVPMatrices[2] = translationMatrix;
            MVPMatrices[3] = viewMatrix;
            MVPMatrices[4] = projectionMatrix;
        }
        void renderAsFaces()
        {
            // Set polygon mode to fill
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            //TODO
        }
        void renderAsWireframe()
        {
            // polygon offset line displaces the vertices towards the camera a little bit
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(0.1, -1);
            // Set polygon mode to line
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            // Actually draw wireframe
            // TODO:

            glDisable(GL_POLYGON_OFFSET_LINE);

        }

        void setVertData()
        {
            
        }

        // genTriangle function
        // Generates a random triangle between global -initialSize and +initialSize
        void genTetrahedron()
        {

        }
};


#endif