

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
#include "Tetrahedron.h"



// SierpinskiPyramid class
class SierpinskiPyramid {
    public:
        SierpinskiPyramid(){}
        void init(GLFWwindow* window, glm::vec3 position, glm::mat4 scale, glm::mat4 rotation)
        {
            renderFaces = true;
            renderWireframe = true;

            // Set up modelMatrix as identity matrix for now
            defaultPosition = position;
            // resetPosition();
            scalingMatrix = scale;
            translationMatrix = glm::translate(position);
            rotationMatrix = rotation;

            // Generate initial point data
            resetPyramid();

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

            // initialize colorType reference in shaders
            colorTypeRef = glGetUniformLocation(tetrahedronShader, "colorType");
            if(colorTypeRef < 0)
            {   std::cerr<< "couldn't find colorType in shader\n"; }

            // Set initial wireframe color
            glUniform3fv(wireframeColorRef, 1, glm::value_ptr(wireframeColor));

            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(1, &buffer);
            glGenBuffers(1, &ibo);

            setVertexBufferData();
            setIndexBufferData();

            // for(int i = 0; i < tetrahedrons.size(); i++)
            // {
            //     for(int j = 0; j < 4; j++)
            //     {
            //         std::cout << " New tetrahedron face\n";
            //         std::cout << "F" << j << ": ";
            //         std::cout << tetrahedrons[i].faces[j].x << ", "
            //             << tetrahedrons[i].faces[j].y << ", "
            //             << tetrahedrons[i].faces[j].z << "\n";
            //     }
            // }        //DEBUG

            // setColorData();

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

            glEnable(GL_CULL_FACE);  
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

            glDisable(GL_CULL_FACE);
            glDisableVertexAttribArray(0);
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
        GLuint tetrahedronShader, buffer, vao, ibo;
        GLint MVPMatrices_ref, wireframeColorRef, colorTypeRef;
        GLFWwindow* window;
        glm::vec3 defaultPosition;
        glm::vec3 wireframeColor = glm::vec3(1.0, 1.0, 1.0);    //Color for the wireframe
        std::vector<glm::vec3> tetrahedronVerts;
        std::vector<Tetrahedron> tetrahedrons;
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
            MVPMatrices[1] = rotationMatrix;
            MVPMatrices[2] = translationMatrix;
            MVPMatrices[3] = viewMatrix;
            MVPMatrices[4] = projectionMatrix;
        }
        void renderAsFaces()
        {  
            // Set polygon mode to fill
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);  

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
                // std::cout << "Vertex: " << vertices[(i*3)+0] << ", "
                //     << vertices[(i*3)+1] << ", "
                //     << vertices[(i*3)+2] << "\n";
            }

            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                3*sizeof(GLfloat)*tetrahedronVerts.size(),
                vertices,
                GL_STATIC_DRAW
            );
            
        }

        void setIndexBufferData()
        {
            unsigned int* triIndices = new unsigned int[tetrahedrons.size()*4*3];
            for(int i = 0; i < tetrahedrons.size(); i++)
            {
                triIndices[(i*4*3)+0] = tetrahedrons[i].faces[0].x;
                triIndices[(i*4*3)+1] = tetrahedrons[i].faces[0].y;
                triIndices[(i*4*3)+2] = tetrahedrons[i].faces[0].z;
                
                triIndices[(i*4*3)+3] = tetrahedrons[i].faces[1].x;
                triIndices[(i*4*3)+4] = tetrahedrons[i].faces[1].y;
                triIndices[(i*4*3)+5] = tetrahedrons[i].faces[1].z;

                triIndices[(i*4*3)+6] = tetrahedrons[i].faces[2].x;
                triIndices[(i*4*3)+7] = tetrahedrons[i].faces[2].y;
                triIndices[(i*4*3)+8] = tetrahedrons[i].faces[2].z;

                triIndices[(i*4*3)+9] = tetrahedrons[i].faces[3].x;
                triIndices[(i*4*3)+10] = tetrahedrons[i].faces[3].y;
                triIndices[(i*4*3)+11] = tetrahedrons[i].faces[3].z;
            }
            // for(int i = 0; i < tetrahedrons.size()*12; i++)
            // {
            //     std::cout << "new index: " << triIndices[i] << " ";
            // }
            // std::cout << std::endl;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, 
                12*sizeof(unsigned int)*tetrahedrons.size(),
                triIndices,
                GL_STATIC_DRAW
                );
        }

        void resetPyramid()
        {
            tetrahedronVerts.clear();
            tetrahedrons.clear();

            tetrahedronVerts.push_back(glm::vec3(0.0, 0.0, 1));
            tetrahedronVerts.push_back(glm::vec3(0.0, 0.942809, -0.33333));
            tetrahedronVerts.push_back(glm::vec3(-0.816497, -0.471405, -0.333333));
            tetrahedronVerts.push_back(glm::vec3(0.816497, -0.471405, -0.333333));

            tetrahedrons.push_back(Tetrahedron(0, 1, 2, 3));
            std::cout << "Resetting pyramids, size=" << tetrahedrons.size() << std::endl;

            setIndexBufferData();
            setVertexBufferData();
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
                // tetrahedronVerts.push_back((tetrahedronVerts[v0] + tetrahedronVerts[v1])/2.0f);
                tetrahedronVerts.push_back(glm::vec3(
                    (tetrahedronVerts[v0].x + tetrahedronVerts[v1].x)/2.0,
                    (tetrahedronVerts[v0].y + tetrahedronVerts[v1].y)/2.0,
                    (tetrahedronVerts[v0].z + tetrahedronVerts[v1].z)/2.0
                ));
                // Save the index of each new vertex
                int v4 = tetrahedronVerts.size()-1;
                // tetrahedronVerts.push_back((tetrahedronVerts[v1] + tetrahedronVerts[v2])/2.0f);
                tetrahedronVerts.push_back(glm::vec3(
                    (tetrahedronVerts[v1].x + tetrahedronVerts[v2].x)/2.0,
                    (tetrahedronVerts[v1].y + tetrahedronVerts[v2].y)/2.0,
                    (tetrahedronVerts[v1].z + tetrahedronVerts[v2].z)/2.0
                ));
                int v5 = tetrahedronVerts.size()-1;
                // tetrahedronVerts.push_back((tetrahedronVerts[v2] + tetrahedronVerts[v0])/2.0f);
                tetrahedronVerts.push_back(glm::vec3(
                    (tetrahedronVerts[v2].x + tetrahedronVerts[v0].x)/2.0,
                    (tetrahedronVerts[v2].y + tetrahedronVerts[v0].y)/2.0,
                    (tetrahedronVerts[v2].z + tetrahedronVerts[v0].z)/2.0
                ));
                int v6 = tetrahedronVerts.size()-1;
                // tetrahedronVerts.push_back((tetrahedronVerts[v0] + tetrahedronVerts[v2])/2.0f);
                tetrahedronVerts.push_back(glm::vec3(
                    (tetrahedronVerts[v0].x + tetrahedronVerts[v3].x)/2.0,
                    (tetrahedronVerts[v0].y + tetrahedronVerts[v3].y)/2.0,
                    (tetrahedronVerts[v0].z + tetrahedronVerts[v3].z)/2.0
                ));
                int v7 = tetrahedronVerts.size()-1;
                // tetrahedronVerts.push_back((tetrahedronVerts[v1] + tetrahedronVerts[v3])/2.0f);
                tetrahedronVerts.push_back(glm::vec3(
                    (tetrahedronVerts[v1].x + tetrahedronVerts[v3].x)/2.0,
                    (tetrahedronVerts[v1].y + tetrahedronVerts[v3].y)/2.0,
                    (tetrahedronVerts[v1].z + tetrahedronVerts[v3].z)/2.0
                ));
                int v8 = tetrahedronVerts.size()-1;
                // tetrahedronVerts.push_back((tetrahedronVerts[v2] + tetrahedronVerts[v3])/2.0f);
                tetrahedronVerts.push_back(glm::vec3(
                    (tetrahedronVerts[v2].x + tetrahedronVerts[v3].x)/2.0,
                    (tetrahedronVerts[v2].y + tetrahedronVerts[v3].y)/2.0,
                    (tetrahedronVerts[v2].z + tetrahedronVerts[v3].z)/2.0
                ));
                int v9 = tetrahedronVerts.size()-1;

                // Add 4 more tetrahedrons in its place
                tetrahedrons.push_back(Tetrahedron(v0, v6, v7, v4));
                tetrahedrons.push_back(Tetrahedron(v1, v8, v5, v4));
                tetrahedrons.push_back(Tetrahedron(v2, v9, v6, v5));
                tetrahedrons.push_back(Tetrahedron(v3, v9, v8, v7));
            }
            for(int i = 0; i < startSize; i++)
            {
                // Remove the base tetrahedrons
                tetrahedrons.erase(tetrahedrons.begin());
            }
            // std::cout << "tetrahedrons.size(): " << tetrahedrons.size() << std::endl;
            setVertexBufferData();
            setIndexBufferData();

        }
};


#endif