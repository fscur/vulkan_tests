#include "screen.h"

#include <iostream>
#include <windows.h>

#define GLM_DEPTH_ZERO_TO_ONE

int main()
{
    auto window = new screen(600, 600, "vulkan rocks!!");
    window->show();
}