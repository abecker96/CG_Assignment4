

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
#include "Primitives.h"



// SierpinskiPyramid class
class SierpinskiPyramid {
    public:
        SierpinskiPyramid(){}
        void init(GLFWwindow* window, glm::vec3 position, glm::mat4 scale, glm::mat4 rotation, glm::vec3 color)
        {
            renderFaces = true;
            renderWireframe = true;
            objectColor = color;

            // Set up modelMatrix as identity matrix for now
            defaultPosition = position;
            // resetPosition();
            scalingMatrix = scale;
            translationMatrix = glm::translate(position);
            rotationMatrix = rotation;

            // Generate initial point data
            resetPyramid();

            // // Load and compile shaders
            pyramidShader = LoadShaders("passthrough.vrt.glsl", "breathingShader.geo.glsl", "breathingShader.frg.glsl");
            glUseProgram(pyramidShader);

            // initialize MVP Matrix array reference in shader
            MVPMatrices_ref = glGetUniformLocation(pyramidShader, "matrices");
            if(MVPMatrices_ref < 0)
            {   std::cerr << "couldn't find MVP matrices in shader\n";  }

            // initialize colorType reference in shaders
            // Used to switch colorTypes in the shader at runtime
            colorTypeRef = glGetUniformLocation(pyramidShader, "colorType");
            if(colorTypeRef < 0)
            {   std::cerr<< "couldn't find colorType in shader\n"; }

            // initialize geoTimer reference in shaders
            // For breathing effect
            geoTimerRef = glGetUniformLocation(pyramidShader, "geoTimer");
            if(geoTimerRef < 0)
            {   std::cerr<< "couldn't find geoTimer in shader\n"; }

            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(1, &positionBuffer);
            glGenBuffers(1, &ibo);
            glGenBuffers(1, &colorBuffer);

            setVertexBufferData();
            setColorBufferData();
            setIndexBufferData();
        }
        // draw function
        // Draws every triangle in the vertexbuffer with a color corresponding to the colorbuffer
        void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            glUseProgram(pyramidShader);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

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
            glUniformMatrix4fv(MVPMatrices_ref, 5, GL_FALSE, glm::value_ptr(MVPMatrices[0])); // Passing 6 matrices

            //set geoTimer
            glUniform1f(geoTimerRef, (float)glfwGetTime());

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
            glDisableVertexAttribArray(1);
        }
        void fractalize()
        {
            fractalizePyramid();
        }
        void reset()
        {
            resetPyramid();
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
            // rotationMatrix = glm::rotate(defaultRotationMatrix, angle, axis);
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
        glm::mat4 translationMatrix, scalingMatrix, rotationMatrix;
        // MVPMatrices will contain the same data as the 5 matrices above
        // The above matrices are just friendly names instead of requiring me to remember index 0,1,2 == O2Wmatrix etc
        glm::mat4 MVPMatrices[5];
        GLuint pyramidShader, positionBuffer, colorBuffer, vao, ibo;
        GLint MVPMatrices_ref, colorTypeRef, geoTimerRef;
        GLFWwindow* window;
        glm::vec3 defaultPosition;
        glm::vec3 objectColor;    //Color for the base shape
        std::vector<glm::vec3> tetrahedronVerts, vertColors;
        std::vector<Tetrahedron> tetrahedrons;
        bool renderFaces, renderWireframe;
        int colorType;
        float currentTime;
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

            // Send colorType for faces to shader
            glUniform1i(colorTypeRef, 1);

            glDrawElements(
                GL_TRIANGLES,
                tetrahedrons.size()*12,
                GL_UNSIGNED_INT,
                (void*)0
            );
        }
        void renderAsWireframe()
        {
            // Set polygon mode to line
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            // polygon offset line displaces the vertices towards the camera a little bit
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(0.1, -1);
                      
            // Send colorType for wireframe to shader
            glUniform1i(colorTypeRef, 0);

            // Actually draw wireframe
            glDrawElements(
                GL_TRIANGLES,
                tetrahedrons.size()*12,
                GL_UNSIGNED_INT,
                (void*)0
            );

            glDisable(GL_POLYGON_OFFSET_LINE);
        }

        void setVertexBufferData()
        {
            GLfloat* vertices = new GLfloat[tetrahedronVerts.size()*3];
            for(int i = 0; i < tetrahedronVerts.size(); i++)
            {
                vertices[(i*3)+0] = tetrahedronVerts[i].x;
                vertices[(i*3)+1] = tetrahedronVerts[i].y;
                vertices[(i*3)+2] = tetrahedronVerts[i].z;
            }

            glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                3*sizeof(GLfloat)*tetrahedronVerts.size(),
                vertices,
                GL_STATIC_DRAW
            );
            delete[] vertices;
        }
        void setColorBufferData()
        {
            GLfloat* vertexColors = new GLfloat[tetrahedronVerts.size()*3];
            for(int i = 0; i < tetrahedronVerts.size(); i++)
            {
                vertexColors[(i*3)+0] = vertColors[i].x;
                vertexColors[(i*3)+1] = vertColors[i].y;
                vertexColors[(i*3)+2] = vertColors[i].z;
            }

            glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                3*sizeof(GLfloat)*tetrahedronVerts.size(),
                vertexColors,
                GL_STATIC_DRAW
            );
            delete[] vertexColors;
        }
        glm::vec3 getColor(glm::vec3 vertexPos)
        {
            static float colorMultiplier = 2;
            return objectColor + objectColor*colorMultiplier*vertexPos.y;
        }
        void setIndexBufferData()
        {
            unsigned int* triIndices = new unsigned int[tetrahedrons.size()*4*3];
            for(int i = 0; i < tetrahedrons.size(); i++)
            {
                for(int j = 0; j < 4; j++)      //j < 4 (faces per tetrahedron)
                {            
                    //        [i*12 indices per tetrahedron +
                    //                  j*3 vertices per face +
                    //                        x, y, z coord in a vertex]
                    triIndices[(i*12) + (j*3)+0] = tetrahedrons[i].faces[j].x;
                    triIndices[(i*12) + (j*3)+1] = tetrahedrons[i].faces[j].y;
                    triIndices[(i*12) + (j*3)+2] = tetrahedrons[i].faces[j].z;
                }
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, 
                12*sizeof(unsigned int)*tetrahedrons.size(),
                triIndices,
                GL_STATIC_DRAW
            );
            delete[] triIndices;
        }

        void resetPyramid()
        {
            tetrahedronVerts.clear();
            vertColors.clear();
            tetrahedrons.clear();

            glm::mat4 pointUpMatrix = glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0));
            glm::vec4 v0 = pointUpMatrix * glm::vec4(0.0, 0.0, 1, 0);
            glm::vec4 v1 = pointUpMatrix * glm::vec4(0.0, 0.942809, -0.33333, 0);
            glm::vec4 v2 = pointUpMatrix * glm::vec4(-0.816497, -0.471405, -0.333333, 0);
            glm::vec4 v3 = pointUpMatrix * glm::vec4(0.816497, -0.471405, -0.333333, 0);


            tetrahedronVerts.push_back(glm::vec3(v0.x, v0.y, v0.z));
            vertColors.push_back(getColor(v0));
            tetrahedronVerts.push_back(glm::vec3(v1.x, v1.y, v1.z));
            vertColors.push_back(getColor(v1));
            tetrahedronVerts.push_back(glm::vec3(v2.x, v2.y, v2.z));
            vertColors.push_back(getColor(v2));
            tetrahedronVerts.push_back(glm::vec3(v3.x, v3.y, v3.z));
            vertColors.push_back(getColor(v3));

            tetrahedrons.push_back(Tetrahedron(0, 1, 2, 3));
            std::cout << "Resetting pyramids, size=" << tetrahedrons.size() << std::endl;

            setVertexBufferData();
            setColorBufferData();
            setIndexBufferData();
        }

        void fractalizePyramid()
        {
            int startSize = tetrahedrons.size();
            for(int i = 0; i < startSize; i++)
            {
                // Save vertex indices for clarity
                int v0 = tetrahedrons[i].verticesIdx[0];
                int v1 = tetrahedrons[i].verticesIdx[1];
                int v2 = tetrahedrons[i].verticesIdx[2];
                int v3 = tetrahedrons[i].verticesIdx[3];

                // Create vertices at midpoints, add to vertex vector
                glm::vec3 newVert = (tetrahedronVerts[v0] + tetrahedronVerts[v1])/2.0f;
                tetrahedronVerts.push_back(newVert);
                vertColors.push_back(getColor(newVert));
                // Save the index of each new vertex
                int v4 = tetrahedronVerts.size()-1;

                newVert = (tetrahedronVerts[v1] + tetrahedronVerts[v2])/2.0f;
                tetrahedronVerts.push_back(newVert);
                vertColors.push_back(getColor(newVert));
                int v5 = tetrahedronVerts.size()-1;

                newVert = (tetrahedronVerts[v2] + tetrahedronVerts[v0])/2.0f;
                tetrahedronVerts.push_back(newVert);
                vertColors.push_back(getColor(newVert));
                int v6 = tetrahedronVerts.size()-1;

                newVert = (tetrahedronVerts[v0] + tetrahedronVerts[v3])/2.0f;
                tetrahedronVerts.push_back(newVert);
                vertColors.push_back(getColor(newVert));
                int v7 = tetrahedronVerts.size()-1;

                newVert = (tetrahedronVerts[v1] + tetrahedronVerts[v3])/2.0f;
                tetrahedronVerts.push_back(newVert);
                vertColors.push_back(getColor(newVert));
                int v8 = tetrahedronVerts.size()-1;

                newVert = (tetrahedronVerts[v2] + tetrahedronVerts[v3])/2.0f;
                tetrahedronVerts.push_back(newVert);
                vertColors.push_back(getColor(newVert));
                int v9 = tetrahedronVerts.size()-1;

                // Add 4 more tetrahedrons
                tetrahedrons.push_back(Tetrahedron(v0, v6, v7, v4));
                tetrahedrons.push_back(Tetrahedron(v1, v8, v5, v4));
                tetrahedrons.push_back(Tetrahedron(v2, v9, v6, v5));
                tetrahedrons.push_back(Tetrahedron(v3, v9, v8, v7));
            }
            for(int i = 0; i < startSize; i++)
            {
                // Remove the base tetrahedrons
                // TODO: why not just do this in the loop with the rest of 'em?
                tetrahedrons.erase(tetrahedrons.begin());
            }
            // std::cout << "tetrahedrons.size(): " << tetrahedrons.size() << std::endl;
            setVertexBufferData();
            setColorBufferData();
            setIndexBufferData();
        }
};


#endif