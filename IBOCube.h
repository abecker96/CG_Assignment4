

#ifndef IBOCUBE_H
#define IBOCUBE_H

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
#include "Primitives.h"

// Cube class
class IBOCube {
    public:
        IBOCube(){}
        void init(GLFWwindow* window, glm::vec3 position, glm::mat4 scale, glm::mat4 rotation, glm::vec3 color)
        {
            // Data initialization
            renderFaces = true;
            renderWireframe = true;
            scalingMatrix = scale;
            currentPosition = position;
            translationMatrix = glm::translate(position);
            rotationMatrix = glm::mat4(1);
            setCubeColors(color);

            // Load and compile shaders
            cubeShader = LoadShaders("o2wShader.vrt.glsl", "passthrough.geo.glsl", "colorShader.frg.glsl");
            glUseProgram(cubeShader);

            // initialize MVP Matrix array reference in shader
            MVPMatrices_ref = glGetUniformLocation(cubeShader, "matrices");
            if(MVPMatrices_ref < 0)
            {   std::cerr << "couldn't find MVP matrices in shader\n";  }

            // initialize wireframe color reference in shaders
            wireframeColorRef = glGetUniformLocation(cubeShader, "wireframeColor");
            if(wireframeColorRef < 0)
            {   std::cerr<< "couldn't find wireframeColor in shader\n"; }

            // initialize colorType reference in shaders
            colorTypeRef = glGetUniformLocation(cubeShader, "colorType");
            if(colorTypeRef < 0)
            {   std::cerr<< "couldn't find colorType in shader\n"; }

            // Set initial wireframe color
            glUniform3fv(wireframeColorRef, 1, glm::value_ptr(wireframeColor));

            //Generate VAO for this cube
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Generate position, color, and IBO buffers for this cube
            glGenBuffers(1, &positionBuffer);
            glGenBuffers(1, &colorBuffer);
            glGenBuffers(1, &ibo);


            setVertexBufferData();
            setColorBufferData();
            setIndexBufferData();
        }
        // draw function
        // Draws every triangle in the vertexbuffer with a color corresponding to the colorbuffer
        void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            // Use shader for the cube
            // Doesn't need to be done every frame unless there are non-cube
            // objects in scene
            glUseProgram(cubeShader);

            // cull backfaces
            glEnable(GL_CULL_FACE);  

            // Use IBO
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

            // Use position buffer
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void*)0
            );

            // Use color buffer
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
            glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void*)0
            );

            // Render relative to the camera
            updateMVPArray(viewMatrix, projectionMatrix);
            glUniformMatrix4fv(MVPMatrices_ref, 5, GL_FALSE, glm::value_ptr(MVPMatrices[0])); // Passing 5 matrices

            // Set wireframe color
            glUniform3fv(wireframeColorRef, 1, glm::value_ptr(wireframeColor));

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

            // Disable cull face in case next object doesn't want to cull faces
            glDisable(GL_CULL_FACE);  

            // disable position/color buffers
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
        }
        //  Toggle wireframe/faces on and off
        void toggleWireframe()
        {
            renderWireframe = !renderWireframe;
        }
        void toggleFaces()
        {
            renderFaces = !renderFaces;
        }
        // Only draw as wireframe/faces
        void drawAsWireframe()
        {
            renderFaces = false;
            renderWireframe = true;
        }
        void drawAsFaces()
        {
            renderFaces = true;
            renderWireframe = false;
        }
        void setRotation(float angle, glm::vec3 axis)
        {
            rotationMatrix = glm::rotate(angle, axis);
        }
        //TODO: rotate function
        void setPosition(const glm::vec3 &position)
        {
            currentPosition = position;
            translationMatrix = glm::translate(glm::mat4(1), position);
        }
        // translate function moves object relative to previous location
        void translate(const glm::vec3 &translation)
        {
            translationMatrix = glm::translate(translationMatrix, translation);
        }
        // returns xyz position in worldspace
        glm::vec3 getPosition()
        {
            return currentPosition;
        }
    private:
        glm::mat4 translationMatrix, scalingMatrix, rotationMatrix;
        // MVPMatrices will contain the same data as the 3 matrices above, as well as passed view & projection matrices
        // The above matrices are just friendly names instead of requiring me to remember index 0 == translationMatrix etc
        glm::mat4 MVPMatrices[5];
        GLuint cubeShader, vao, ibo, positionBuffer, colorBuffer;
        GLint MVPMatrices_ref, wireframeColorRef, colorTypeRef;     //glUniform location references for shader
        GLFWwindow* window;
        glm::vec3 wireframeColor = glm::vec3(1.0, 1.0, 1.0);    //Color for the wireframe
        glm::vec3 currentPosition;      //xyz position instead of pure translation matrix
        GLfloat cubeColors[24];         //color data for each vertex
        GLfloat cubeVerts[24] = {       //Basic cube coordinates
            0.0f, 0.0f, 0.0f,           //TODO: center on origin for easy positioning in worldspace
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f
        };
        unsigned int cubeIndices[36];   // Indices to be passed to IBO
        bool renderFaces, renderWireframe;
        // update existing MVP array from matrices with friendly names
        void updateMVPArray(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            // This order is really important
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

            // Send colorType for faces to shader
            glUniform1i(colorTypeRef, 1);

            glDrawElements(
                GL_TRIANGLES,
                36,     //6 quads * 2 triangles per quad * 3 indices per triangle
                GL_UNSIGNED_INT,
                (void*)0
            );

            glDisable(GL_CULL_FACE);
        }
        void renderAsWireframe()
        {
            // polygon offset line displaces the vertices towards the camera a little bit
            glEnable(GL_POLYGON_OFFSET_LINE);

            glPolygonOffset(0.1, -1);
            // Set polygon mode to line
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                      
            // Send colorType for wireframe to shader
            glUniform1i(colorTypeRef, 0);

            // Actually draw wireframe
            glDrawElements(
                GL_TRIANGLES,
                36,     //6 quads * 2 triangles per quad * 3 indices per triangle
                GL_UNSIGNED_INT,
                (void*)0
            );

            glDisable(GL_POLYGON_OFFSET_LINE);
        }
        // Generates color array from a given color
        void setCubeColors(glm::vec3 color)
        {
            for(int i = 0; i < 8; i++)
            {
                cubeColors[i*3+0] = color.x;
                cubeColors[i*3+1] = color.y;
                cubeColors[i*3+2] = color.z;
            }
        }
        // Sets VBO data from vertex position array
        void setVertexBufferData()
        {
            glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                24*sizeof(GLfloat),
                cubeVerts,
                GL_STATIC_DRAW
            );
        }
        // Sets color buffer data from color array
        void setColorBufferData()
        {
            glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                24*sizeof(GLfloat),
                cubeColors,
                GL_STATIC_DRAW
            );
        }
        // Generate IBO
        void setIndexBufferData()
        {
            // I'll be honest here, Cube from Primitives.h isn't necessary here
            // But this current IBOCube class started off as a MengerSponge, where it was necessary
            // I just didn't want to re-write the code here
            Cube cube = Cube(0, 1, 2, 3, 4, 5, 6, 7);
            for(int i = 0; i < 6; i++)
            {
                cubeIndices[i*6 + 0] = cube.quads[i].faces[0].x;
                cubeIndices[i*6 + 1] = cube.quads[i].faces[0].y;
                cubeIndices[i*6 + 2] = cube.quads[i].faces[0].z;
                cubeIndices[i*6 + 3] = cube.quads[i].faces[1].x;
                cubeIndices[i*6 + 4] = cube.quads[i].faces[1].y;
                cubeIndices[i*6 + 5] = cube.quads[i].faces[1].z;
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, 
                36*sizeof(unsigned int),      //6 quads, 2 triangles per quad, 3 vertices (indices) per triangle
                cubeIndices,
                GL_STATIC_DRAW
            );
        }
};

#endif