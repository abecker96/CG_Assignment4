

#ifndef MENGERSPONGE_H
#define MENGERSPONGE_H

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



// MengerSponge class
class MengerSponge {
    public:
        MengerSponge(){}
        void init(GLFWwindow* window, glm::vec3 position, glm::mat4 scale, glm::mat4 rotation)
        {
            renderFaces = true;
            renderWireframe = true;

            // Set up modelMatrix as identity matrix for now
            defaultPosition = position;
            // resetPosition();
            scalingMatrix = scale;
            translationMatrix = glm::translate(position);
            rotationMatrix = glm::mat4(1);
            defaultRotationMatrix = rotation;

            // Generate initial point data
            resetSponge();

            // // Load and compile shaders
            baseShader = LoadShaders("baseShader.vrt.glsl", "baseShader.geo.glsl", "baseShader.frg.glsl");
            glUseProgram(baseShader);

            // initialize MVP Matrix array reference in shader
            MVPMatrices_ref = glGetUniformLocation(baseShader, "matrices");
            if(MVPMatrices_ref < 0)
            {   std::cerr << "couldn't find MVP matrices in shader\n";  }

            // initialize wireframe color reference in shaders
            wireframeColorRef = glGetUniformLocation(baseShader, "wireframeColor");
            if(wireframeColorRef < 0)
            {   std::cerr<< "couldn't find wireframeColor in shader\n"; }

            // initialize colorType reference in shaders
            colorTypeRef = glGetUniformLocation(baseShader, "colorType");
            if(colorTypeRef < 0)
            {   std::cerr<< "couldn't find colorType in shader\n"; }

            // initialize geoTimer reference in shaders
            geoTimerRef = glGetUniformLocation(baseShader, "geoTimer");
            if(geoTimerRef < 0)
            {   std::cerr<< "couldn't find geoTimer in shader\n"; }

            // Set initial wireframe color
            glUniform3fv(wireframeColorRef, 1, glm::value_ptr(wireframeColor));

            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(1, &buffer);
            glGenBuffers(1, &ibo);

            setVertexBufferData();
            setIndexBufferData();
            // setColorData();      //TODO

        }
        // draw function
        // Draws every triangle in the vertexbuffer with a color corresponding to the colorbuffer
        void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            // Render relative to the camera
            updateMVPArray(viewMatrix, projectionMatrix);
            glUniformMatrix4fv(MVPMatrices_ref, 5, GL_FALSE, glm::value_ptr(MVPMatrices[0])); // Passing 6 matrices

            // Set wireframe color
            glUniform3fv(wireframeColorRef, 1, glm::value_ptr(wireframeColor));

            // update timer for explodey effect
            glUniform1f(geoTimerRef, (float)glfwGetTime());

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void*)0
            );

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

            glDisableVertexAttribArray(0);
        }
        void fractalize()
        {
            fractalizeSponge();
        }
        void reset()
        {
            resetSponge();
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
            // rotationMatrix = glm::rotate(rotationMatrix, angle, axis);
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
        glm::mat4 objectToWorldMatrix, translationMatrix, scalingMatrix, rotationMatrix, defaultRotationMatrix;
        // MVPMatrices will contain the same data as the 5 matrices above
        // The above matrices are just friendly names instead of requiring me to remember index 0 == O2Wmatrix etc
        glm::mat4 MVPMatrices[5];
        GLuint baseShader, buffer, vao, ibo;
        GLint MVPMatrices_ref, wireframeColorRef, colorTypeRef, geoTimerRef;
        GLFWwindow* window;
        glm::vec3 defaultPosition;
        glm::vec3 wireframeColor = glm::vec3(1.0, 1.0, 1.0);    //Color for the wireframe
        std::vector<glm::vec3> spongeVerts;
        std::vector<Cube> cubes;
        bool renderFaces, renderWireframe;
        int colorType;
        float currentTime;
        // color palettes
        const float colorPalettes[3] = {
            1.0000, 1.0000, 1.0000
        };
        void updateMVPArray(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            // This order is *really* important
            MVPMatrices[0] = scalingMatrix;
            MVPMatrices[1] = rotationMatrix * defaultRotationMatrix;
            MVPMatrices[2] = translationMatrix;
            MVPMatrices[3] = viewMatrix;
            MVPMatrices[4] = projectionMatrix;
        }
        void renderAsFaces()
        {  
            glEnable(GL_CULL_FACE);  

            // Set polygon mode to fill
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);  

            // Send colorType for faces to shader
            glUniform1i(colorTypeRef, 1);

            glDrawElements(
                GL_TRIANGLES,
                cubes.size()*6*2*3,     //6 quads * 2 triangles per quad * 3 indices per triangle
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
                cubes.size()*6*2*3,     //6 quads * 2 triangles per quad * 3 indices per triangle
                GL_UNSIGNED_INT,
                (void*)0
            );

            glDisable(GL_POLYGON_OFFSET_LINE);

        }

        void setVertexBufferData()
        {
            GLfloat* vertices = new GLfloat[spongeVerts.size()*3];
            for(int i = 0; i < spongeVerts.size(); i++)
            {
                vertices[i*3+0] = spongeVerts[i].x;
                vertices[i*3+1] = spongeVerts[i].y;
                vertices[i*3+2] = spongeVerts[i].z;
            }

            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                3*sizeof(GLfloat)*spongeVerts.size(),
                vertices,
                GL_STATIC_DRAW
            );
            
        }

        void setIndexBufferData()
        {
            unsigned int* quadIndices = new unsigned int[cubes.size()*12*3];
            // loop through every cube
            for(int i = 0; i < cubes.size(); i++)
            {
                // loop through every quad in a cube
                for(int j = 0; j < 6; j++)
                {
                    //         [i*36 indices per cube +
                    //               j*6 quads per cube]
                    quadIndices[i*36+j*6+0] = cubes[i].quads[j].faces[0].x;
                    quadIndices[i*36+j*6+1] = cubes[i].quads[j].faces[0].y;
                    quadIndices[i*36+j*6+2] = cubes[i].quads[j].faces[0].z;
                    quadIndices[i*36+j*6+3] = cubes[i].quads[j].faces[1].x;
                    quadIndices[i*36+j*6+4] = cubes[i].quads[j].faces[1].y;
                    quadIndices[i*36+j*6+5] = cubes[i].quads[j].faces[1].z;
                }
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, 
                12*3*sizeof(unsigned int)*cubes.size(),      //6 faces, 4 vertices (indices) each
                quadIndices,
                GL_STATIC_DRAW
            );
        }

        void resetSponge()
        {
            spongeVerts.clear();
            cubes.clear();

            spongeVerts = {
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 1.0f),
                glm::vec3(1.0f, 1.0f, 1.0f),
                glm::vec3(1.0f, 0.0f, 1.0f)
            };

            cubes.push_back(Cube(0, 1, 2, 3, 4, 5, 6, 7));
            std::cout << "Resetting sponge, size=" << cubes.size() << std::endl;

            setIndexBufferData();
            setVertexBufferData();
        }

        void fractalizeSponge()
        {
            int startSize = cubes.size();
            // loop through all initial cubes
            for(int i = 0; i < startSize; i++)
            {
                // int v0 = cubes[i].verticesIdx[0];

            }
            for(int i = 0; i < startSize; i++)
            {

            }
            // std::cout << "tetrahedrons.size(): " << tetrahedrons.size() << std::endl;
            setVertexBufferData();
            setIndexBufferData();

        }
};


#endif