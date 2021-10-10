#ifndef TETRAHEDRON_H
#define TETRAHEDRON_H

#include <glm/glm.hpp>
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

            // std::cout << "new tetrahedron indices: ";
            // for(int i = 0; i < 4; i++)
            // {
            //     std::cout << faces[i].x << " ";
            //     std::cout << faces[i].y << " ";
            //     std::cout << faces[i].z << " ";
            // }
            // std::cout << std::endl;
        }
        // Store all indices used in this tetrahedron
        int verticesIdx[4];
        // Store the relationship between faces and indices with correct spin
        glm::ivec3 faces[4];
};

#endif