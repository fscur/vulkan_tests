#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class camera
{
private:
    int _width;
    int _height;
public:
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 gaze;
    glm::vec3 position;
    glm::vec3 target;
public:
    camera(glm::vec3 position, glm::vec3 target, int width, int height);
    ~camera() {};

    void update();
    void resize(int width, int height);

    void translate(glm::vec3 translation);
    void orbit(glm::vec3 origin, glm::vec3 axis, float angle);
    void zoom(float amount);
    void pan(glm::vec3 axis, float amount);

    inline glm::mat4 getProjectionMatrix() const { return glm::perspective<float>(glm::half_pi<float>(), (float)(_width / _height), 0.1f, 100.0f); }
    inline glm::mat4 getViewMatrix() const { return glm::lookAt<float>(position, target, up); }
};

