

#ifndef FRACTALMESH_H
#define FRACTALMESH_H

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
#include "Vector3.h"
#include "LoadShaders.h"

// FractalMesh class
class FractalMesh {
    public:
        FractalMesh(){}
        void init(GLFWwindow* window, glm::vec3 position, glm::mat4 scale, glm::mat4 rotation, int colorStyle, bool randomColors)
        {
            renderFaces = true;
            renderWireframe = true;
            colorType = colorStyle;
            colorType;
            variableColors = randomColors;

            glGenVertexArrays(1, &VertexArrayID);
            glBindVertexArray(VertexArrayID);

            // Load and compile shaders
            fractalShader = LoadShaders("fractalShader.vrt.glsl", "fractalShader.frg.glsl");
            glUseProgram(fractalShader);

            // initialize MVP Matrix array reference in shader
            MVPMatrices_ref = glGetUniformLocation(fractalShader, "matrices");
            if(MVPMatrices_ref < 0)
            {   std::cerr << "couldn't find MVP matrices in shader\n";  }

            // initialize wireframe color reference in shaders
            wireframeColorRef = glGetUniformLocation(fractalShader, "wireframeColor");
            if(wireframeColorRef < 0)
            {   std::cerr<< "couldn't find wireframeColor in shader\n"; }

            // initialize timer in rainbow reference shaders
            timerRef = glGetUniformLocation(fractalShader, "colorTimer");
            if(timerRef < 0)
            {   std::cerr<< "couldn't find colorTimer in shader\n";   }

            // initialize colorType reference in faces shaders
            colorTypeRef = glGetUniformLocation(fractalShader, "colorType");
            if(colorTypeRef < 0)
            {   std::cerr<< "couldn't find colorType in shader\n";   }

            // Generate initial triangle
            fractalVertVec = genTriangle();

            // Generate vertex buffer
            glGenBuffers(1, &vertexbuffer);
            // Generate color buffer
            glGenBuffers(1, &colorbuffer);

            // Set initial vertex data
            // This also sends data to the GPU
            setVertData();
            // Set initial color data
            // This also sends data to the GPU
            setColorData();

            // Set up modelMatrix as identity matrix for now
            defaultPosition = position;
            resetPosition();
            scalingMatrix = scale;
            rotationMatrix = rotation;
            rotateOriginMatrix = glm::mat4(1);

            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            // Activating attribute buffers and sending updated data to the GPU
            // 1st attribute buffer : vertex positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(
                0,                  //matches layout in vertex shader
                3,                  //3 vertices
                GL_FLOAT,           //type
                GL_FALSE,           //normalized?
                0,                  //stride
                (void*)0            //array buffer offset
            );

            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
            // 2nd attribute buffer : colors
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(
                1,                                // matches layout in fragment shader
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );
        }
        // draw function
        // Draws every triangle in the vertexbuffer with a color corresponding to the colorbuffer
        void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {

            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            // Activating attribute buffers and sending updated data to the GPU
            // 1st attribute buffer : vertex positions
            // glEnableVertexAttribArray(0);
            glVertexAttribPointer(
                0,                  //matches layout in vertex shader
                3,                  //3 vertices
                GL_FLOAT,           //type
                GL_FALSE,           //normalized?
                0,                  //stride
                (void*)0            //array buffer offset
            );

            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
            // 2nd attribute buffer : colors
            // glEnableVertexAttribArray(1);
            glVertexAttribPointer(
                1,                                // matches layout in fragment shader
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // Render relative to the camera
            updateMVPArray(viewMatrix, projectionMatrix);
            glUniformMatrix4fv(MVPMatrices_ref, 6, GL_FALSE, glm::value_ptr(MVPMatrices[0])); // Passing 6 matrices

            // Send colorTimer to shader
            currentTime = (float)glfwGetTime();
            glUniform1fv(timerRef, 1, &currentTime);

            // Send colorType to shader
            glUniform1i(colorTypeRef, colorType);

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
        }
        void fractalize()
        {
            // Generate more triangles
            fractalVertVec = fractalizeTriangles(fractalVertVec, displacement);
            // Lower displacement value for each level of detail
            displacement *= roughness;

            setVertData();
            setColorData();
        }
        void reset(int seed)
        {
            // Re-initialize random number generator
            seed = (int)(glfwGetTime()*1000)+seed;
            srand(seed);
            // Output seed to console for potential replication
            std::cout << "Seed: " << seed << std::endl;

            // Create a new random triangle
            fractalVertVec = genTriangle();
            // Reset displacement value to default
            displacement = startDisplacement;

            setVertData();
            setColorData();
        }
        void setColor(int idx)
        {
            colorSelector = idx;
            setColorData();
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
        void rotateAroundOrigin(float angle, glm::vec3 axis)
        {
            rotateOriginMatrix = glm::rotate(angle, axis);
        }
        void translate(const glm::vec3 &translation)
        {
            translationMatrix = glm::translate(translationMatrix, translation);
        }
        void setPosition(const glm::vec3 &position)
        {
            translationMatrix = glm::translate(glm::mat4(1), position);
        }
        void resetPosition()
        {
            setPosition(defaultPosition);
        }
    private:
        glm::mat4 objectToWorldMatrix, translationMatrix, scalingMatrix, rotationMatrix, rotateOriginMatrix;
        glm::mat4 MVPMatrices[6];
        GLuint fractalShader, VertexArrayID;
        GLuint vertexbuffer, colorbuffer;
        GLint MVPMatrices_ref, wireframeColorRef, timerRef, colorTypeRef;
        GLFWwindow* window;
        glm::vec3 defaultPosition;
        glm::vec3 wireframeColor = glm::vec3(1.0, 1.0, 1.0);    //Color for the wireframe
        std::vector<Vector3> fractalVertVec;
        int seed, colorSelector, colorType;
        bool renderFaces, renderWireframe, variableColors;
        float roughness = 0.5;                //How quickly displacement decreases
        float startDisplacement = 1.2;        //affects initial displacement from starting vertex
        float displacement = startDisplacement;     //changes every iteration
        float currentTime;
        // A malleable color data array.
        float currentColorPalette[9];
        // color data
        const float colorPalettes[36] = {
            0.1500, 0.1500, 0.3647,     //color 1   //Blue color palette
            0.7400, 0.7500, 0.7800,     //color 2
            0.5900, 0.6000, 0.4153,     //color2-color1 for scaling
            0.3215, 0.2470, 0.4941,     //purple color palette
            0.4431, 0.4863, 0.7568,
            0.1216, 0.2393, 0.2627,
            0.4000, 0.7000, 0.3000,     //green color palette
            0.7686, 0.8274, 0.8941,
            0.2745, 0.0431, 0.5804,     
            0.0000, 0.0000, 0.0000,     //green-est color palette
            0.0000, 1.0000, 0.0000,
            0.0000, 1.0000, 0.0000
        };
        void updateMVPArray(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            // This order is *really* important
            MVPMatrices[0] = scalingMatrix;
            MVPMatrices[1] = rotationMatrix;
            MVPMatrices[2] = translationMatrix;
            MVPMatrices[3] = rotateOriginMatrix;
            MVPMatrices[4] = viewMatrix;
            MVPMatrices[5] = projectionMatrix;
        }
        void renderAsFaces()
        {
            // Set polygon mode to fill
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawArrays(GL_TRIANGLES, 0, fractalVertVec.size()); //draw 3 vertices as a triangle
        }
        void renderAsWireframe()
        {
            // Send colorType for wireframe
            glUniform1i(colorTypeRef, 0);

            // polygon offset line displaces the vertices towards the camera a little bit
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(0.1, -1);

            // Actually draw wireframe
            // Set polygon mode to line
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_TRIANGLES, 0, fractalVertVec.size()); //draw 3 vertices as a triangle

            glDisable(GL_POLYGON_OFFSET_LINE);

        }
        // fractalizeTriangle function
        // Takes a <Vector3> vector, starting index, and a displacement scalar as input
        // Assumes that a, b, and c are 3 consecutive points in the input vector, starting from the startIdx
        //  calls displace() to generate 3 semi random points, assembles those points into 4 new triangles
        //  such that each triangle is composed of 3 consecutive points in the output vector.
        std::vector<Vector3> fractalizeTriangle(const std::vector<Vector3> &inVec, int startIdx, float displacement) {
            std::vector<Vector3> returnVector;
            //          c
            //      f       e
            //  a       d       b
            Vector3 a = inVec[startIdx];
            Vector3 b = inVec[startIdx + 1];
            Vector3 c = inVec[startIdx + 2];
            Vector3 d = displace(a, b, displacement);
            Vector3 e = displace(b, c, displacement);
            Vector3 f = displace(c, a, displacement);

            //first triangle
            returnVector.push_back(d);
            returnVector.push_back(a);
            returnVector.push_back(f);

            //second triangle
            returnVector.push_back(b);
            returnVector.push_back(d);
            returnVector.push_back(e);

            //third triangle
            returnVector.push_back(e);
            returnVector.push_back(f);
            returnVector.push_back(c);

            //fourth triangle
            returnVector.push_back(e);
            returnVector.push_back(d);
            returnVector.push_back(f);

            return returnVector;
        }

        // fractalizeTriangles function
        // Takes a vector of vertices, assuming that 3 consecutive vertices create a triangle.
        // Loops through every triangle, and calls fractalizeTriangle on it
        // Appends output of each iteration to the output vector
        std::vector<Vector3> fractalizeTriangles(const std::vector<Vector3> &inVec, float displacement)
        {
            std::vector<Vector3> outVec;
            std::vector<Vector3> tempVec;
            for(int i = 0; i+2 < inVec.size(); i+=3)
            {
                tempVec = fractalizeTriangle(inVec, i, displacement);
                outVec.insert(outVec.end(), tempVec.begin(), tempVec.end());
            }
            return outVec;
        }

        // generateColor function
        // Takes a point and a colorPalette as input
        // Returns a point3d where color is scaled between the two colors in the colorpalette
        //  based on the point's y position
        Vector3 generateColor(const Vector3 &p, const float* colorPalette)
        {
            float colorScale = (p.y+1)/2;
            float random = randomBetween(0, 1);
            if(!variableColors)
            {   
                random = 1; 
                colorScale = 1;
            }
            float g = colorPalette[1] + random*colorScale*colorPalette[7];
            float b = colorPalette[2] + random*colorScale*colorPalette[8];
            float r = colorPalette[0] + random*colorScale*colorPalette[6];
            return Vector3(r, g, b);
        }

        // setColorData function
        // Copies color data from generateColor into the appropriate location
        //  of an array
        void setColorArray(float *colorArr, const std::vector<Vector3> &source)
        {
            Vector3 c = Vector3(1, 1, 1);
            for(int i = 0; i < source.size(); i++)
            {
                c = generateColor(source[i], currentColorPalette);
                colorArr[(i*3)] = c.x;      // red
                colorArr[(i*3)+1] = c.y;    // green
                colorArr[(i*3)+2] = c.z;    // blue
            }
        }

        void setColorData()
        {
            //set color palette for this update
            for(int i = 0; i < 9; i++)
            {
                currentColorPalette[i] = colorPalettes[i+colorSelector*9];
            }

            // set colors for each vertex (for drawing faces)
            GLfloat *colorData = new GLfloat[fractalVertVec.size()*3];
            // reset random number generator so the colors are the same every frame
            srand(seed);
            // generate colors, load into colorData array
            setColorArray(colorData, fractalVertVec);

            // send colorData array to gpu
            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*fractalVertVec.size()*3, colorData, GL_STATIC_DRAW);

            // Set wireframe color
            glUniform3fv(wireframeColorRef, 1, glm::value_ptr(wireframeColor));

            // Initialize colorTimer in shader
            currentTime = (float)glfwGetTime();
            glUniform1fv(timerRef, 1, &currentTime);

            // Send colorType to shader
            glUniform1i(colorTypeRef, colorType);

            delete[] colorData;
        }

        void setVertData()
        {
            GLfloat *g_vertex_buffer_data = new GLfloat[fractalVertVec.size()*3];
            // GLfloat g_color_buffer_data[fractalVertVec.size()*3];
            pointVecToArr(g_vertex_buffer_data, fractalVertVec);

            // Bind vertex buffer as an array buffer
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            // Dump vertex data to graphics card
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*fractalVertVec.size()*3, g_vertex_buffer_data, GL_STATIC_DRAW);

            delete[] g_vertex_buffer_data;
        }

        // pointVecToArr function to easily take a vector containing triangle vertices
        //  and copy those vertices to the correct position of an array
        void pointVecToArr(float * vertArr, const std::vector<Vector3> &source)
        {
            for(int i = 0; i < source.size(); i++)
            {
                vertArr[(i*3)] = source[i].x;
                vertArr[(i*3)+1] = source[i].y;
                vertArr[(i*3)+2] = source[i].z;
            }
        }

        // displace function
        // Takes two point3d vertices, generates a midpoint between them, and
        //  randomly displaces that vertex based on a global displacement value
        Vector3 displace(Vector3 &a, Vector3 &b, float displacement) {
            Vector3 midpoint = Vector3((a+b)/2.0);

            srand(midpoint.x*midpoint.y*seed);
            float random = (((float) rand()) / (float) RAND_MAX) - 0.5;
            midpoint.x = midpoint.x + displacement*random;

            random = (((float) rand()) / (float) RAND_MAX) - 0.5;
            midpoint.y = midpoint.y + displacement*random;

            random = (((float) rand()) / (float) RAND_MAX) - 0.5;
            midpoint.z = midpoint.z + displacement*random;

            return midpoint;
        }
        // https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats/5289624
        // Generates a random float between two values.
        // Does not care if the largest value is first or last
        float randomBetween(float a, float b) {
            if(a > b) {
                float temp = a;
                a = b;
                b = temp;
            }
            float random = ((float) rand()) / (float) RAND_MAX;
            float diff = b-a;
            float r = random * diff;
            return a + r;
        }
        // genTriangle function
        // Generates a random triangle between global -initialSize and +initialSize
        std::vector<Vector3> genTriangle()
        {
            seed = (int)(glfwGetTime()*10000);
            std::vector<Vector3> newTriangle = {
                Vector3(randomBetween(-1, 1), randomBetween(-1, 1), randomBetween(-1, 1)),
                Vector3(randomBetween(-1, 1), randomBetween(-1, 1), randomBetween(-1, 1)),
                Vector3(randomBetween(-1, 1), randomBetween(-1, 1), randomBetween(-1, 1))
            };
            return newTriangle;
        }
};


#endif