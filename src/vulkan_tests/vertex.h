#pragma once

#include <glm\glm.hpp>

struct vertex
{
    vertex(glm::vec3 position, glm::vec2 texCoord, glm::vec3 normal) : 
        position(position),
        texCoord(texCoord),
        normal(normal)
    {
    }

    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};