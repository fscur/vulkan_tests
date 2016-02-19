#include "window.h"

#include "vulkan.h"

#include <iostream>
#include <windows.h>

window* _window;

int main()
{
    _window = new window(600, 600, "vulkan rocks!!");

    auto vk = new vulkan(_window);

    std::cout << "running" << std::endl;
    _window->run();
}