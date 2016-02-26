#pragma once

#include <glm\glm.hpp>

typedef struct vertex
{
    vertex(glm::vec3 position, glm::vec3 normal) : 
        position(position),
        normal(normal)
    {
    }

    glm::vec3 position;
    glm::vec3 normal;
};