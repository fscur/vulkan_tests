#pragma once

#include <glm\glm.hpp>

class mouseEventArgs
{
public:
    enum mouseButtons
    {
        none = 0,
        left = 1,
        middle = 2,
        right = 4,
    };
public:
    mouseButtons pressedButtons;
    glm::vec2 position;
public:
    mouseEventArgs() :
        pressedButtons(mouseButtons::none),
        position(glm::vec2(0.0f))
    {
    }

    mouseEventArgs(mouseButtons pressedButtons, glm::vec2 position) :
        pressedButtons(pressedButtons),
        position(position)
    {
    }

    ~mouseEventArgs() {}
};

