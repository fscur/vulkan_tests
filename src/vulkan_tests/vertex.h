#pragma once

#include <glm\glm.hpp>

struct vertex
{
    glm::vec3 position;

    vertex(glm::vec3 position) :
        position(position)
    {
    };
};