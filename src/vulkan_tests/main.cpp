#include "screen.h"
#include "vulkan.h"

#include <iostream>
#include <windows.h>

int main()
{
    auto window = new screen(600, 600, "vulkan rocks!!");
    window->show();
}