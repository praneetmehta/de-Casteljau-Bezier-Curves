#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>

class Triangle
{
public:
    glm::vec3 A;
    glm::vec3 B;
    glm::vec3 C;

    Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
    {
        A = v1;
        B = v2;
        C = v3;
    }
};

#endif
