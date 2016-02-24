#include "camera.h"

#include "mathUtils.h"

#include <iostream>

camera::camera(glm::vec3 position, glm::vec3 target, int width, int height) :
    position(position),
    target(target),
    _width(width),
    _height(height)
{
    update();
}

void camera::update()
{
    gaze = glm::normalize(target - position);
    right = glm::cross(gaze, glm::vec3(0.0f, 1.0f, 0.0f));
    up = glm::cross(right, gaze);

    right = glm::normalize(right);
    up = glm::normalize(up);
}

void camera::resize(int width, int height)
{
    _width = width;
    _height = height;
}

void camera::translate(glm::vec3 translation)
{
    position += translation;
    target += translation;
    update();
}

void camera::orbit(glm::vec3 origin, glm::vec3 axis, float angle)
{
    auto newPos = mathUtils::rotateAboutAxis(position, origin, axis, angle);
    auto newGaze = glm::normalize(target - newPos);

    auto diff = glm::dot(newGaze, glm::vec3(0.0f, 1.0f, 0.0f));
    if (diff >= 0.999 || diff <= -0.999)
        return;

    position = newPos;
    update();
}

void camera::zoom(float amount)
{
    position += gaze * amount;
    update();
}

void camera::pan(glm::vec3 axis, float amount)
{
    translate(axis * amount);
}