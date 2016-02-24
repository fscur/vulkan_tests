#include "screen.h"

#include <iostream>

screen::screen(int width, int height, char* title) :
    window::window(width, height, title),
    _lastMousePosition(glm::vec2(0.0f)),
    _rotating(false),
    _panning(false)
{
}

void screen::onLoaded()
{
    _camera = new camera(glm::vec3(0.0, 1.5f, 1.5f), glm::vec3(0.0f), _width, _height);
    _vulkan = new vulkan(_handle, _instance, _width, _height);
}

void screen::onMouseUp(mouseEventArgs* eventArgs)
{
    _rotating = false;
    _panning = false;
}

void screen::onMouseDown(mouseEventArgs* eventArgs)
{
    _lastMousePosition = eventArgs->position;
    switch (eventArgs->pressedButtons)
    {
    case mouseEventArgs::mouseButtons::left:
        _panning = true;
        break;
    case mouseEventArgs::mouseButtons::right:
        _rotating = true;
        break;
    default:
        break;
    }
}

void screen::onMouseMove(mouseEventArgs* eventArgs)
{
    auto currentPosition = eventArgs->position;
    auto delta = (_lastMousePosition - currentPosition) * 0.01f;

    if (_rotating)
    {
        _camera->orbit(glm::vec3(0.0f), _camera->up, delta.x);
        _camera->orbit(glm::vec3(0.0f), _camera->right, delta.y);
    }
    else if (_panning)
    {
        _camera->pan(_camera->right, delta.x);
        _camera->pan(_camera->up, -delta.y);
    }

    _lastMousePosition = currentPosition;
}

void screen::loop()
{
    while (!_shouldExit)
    {
        handleMessages();
        update();
        render();
    }
}

void screen::update()
{
    //_camera->orbit(glm::vec3(), _camera->up, 0.001);

    _vulkan->setFrameUniforms(_camera->getViewMatrix(), _camera->getProjectionMatrix());
}

void screen::render()
{
    _vulkan->buildCommandBuffer();
    _vulkan->draw();
}