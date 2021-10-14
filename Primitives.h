#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/glm.hpp>

//unused, might store normals in the future
// but everything is currently abstracted too far to be useful anymore
// so... refactor everything here later
struct Tri {
    public:
        Tri(){}
        Tri(int v1, int v2, int v3)
        {
            vertices = glm::ivec3(v1, v2, v3);
        }
        glm::ivec3 vertices;
};

struct Quad {
    public:
        Quad(){}
        Quad(int v0, int v1, int v2, int v3)
        {
            verticesIdx[0] = v0;
            verticesIdx[1] = v1;
            verticesIdx[2] = v2;
            verticesIdx[3] = v3;
            
            faces[0] = glm::ivec3(verticesIdx[2], verticesIdx[1], verticesIdx[0]);
            faces[1] = glm::ivec3(verticesIdx[3], verticesIdx[2], verticesIdx[0]);
        }

        // Store all indices used in this cube
        int verticesIdx[4];
        // Store the relationship between faces and indices with correct spin
        glm::ivec3 faces[2];
};

struct Cube {
    public:
        Cube(int v0, int v1, int v2, int v3, int v4, int v5, int v6, int v7)
        {
            verticesIdx[0] = v0;
            verticesIdx[1] = v1;
            verticesIdx[2] = v2;
            verticesIdx[3] = v3;
            verticesIdx[4] = v4;
            verticesIdx[5] = v5;
            verticesIdx[6] = v6;
            verticesIdx[7] = v7;
            
            quads[0] = Quad(verticesIdx[2], verticesIdx[3], verticesIdx[0], verticesIdx[1]);
            quads[1] = Quad(verticesIdx[6], verticesIdx[2], verticesIdx[1], verticesIdx[7]);
            quads[2] = Quad(verticesIdx[5], verticesIdx[6], verticesIdx[7], verticesIdx[4]);
            quads[3] = Quad(verticesIdx[3], verticesIdx[5], verticesIdx[4], verticesIdx[0]);
            quads[4] = Quad(verticesIdx[0], verticesIdx[4], verticesIdx[7], verticesIdx[1]);
            quads[5] = Quad(verticesIdx[2], verticesIdx[6], verticesIdx[5], verticesIdx[3]);
        }

        // Store all indices used in this cube
        int verticesIdx[8];
        // Store the relationship between faces and indices with correct spin
        Quad quads[6];
};

struct Tetrahedron {
    public:
        Tetrahedron(int v0, int v1, int v2, int v3)
        {
            verticesIdx[0] = v0;
            verticesIdx[1] = v1;
            verticesIdx[2] = v2;
            verticesIdx[3] = v3;

            faces[0] = glm::ivec3(verticesIdx[0], verticesIdx[1], verticesIdx[2]);
            faces[1] = glm::ivec3(verticesIdx[3], verticesIdx[2], verticesIdx[1]);
            faces[2] = glm::ivec3(verticesIdx[0], verticesIdx[2], verticesIdx[3]);
            faces[3] = glm::ivec3(verticesIdx[0], verticesIdx[3], verticesIdx[1]);
        }
        // Store all indices used in this tetrahedron
        int verticesIdx[4];
        // Store the relationship between faces and indices with correct spin
        glm::ivec3 faces[4];
};
#endif