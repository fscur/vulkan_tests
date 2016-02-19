#include "window.h"

#include <iostream>

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        //run(); //input ??
        return 0;
        break;
    case WM_KEYDOWN:
        break;
    case WM_KEYUP:
        break;
    case WM_NCMOUSEMOVE:
        break;
    default:
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

window::window(int width, int height, char* title) :
    _instance(GetModuleHandle(NULL)),
    _title(title),
    _width(width),
    _height(height)
{
    WNDCLASSEX winClass;
    winClass.cbSize = sizeof(WNDCLASSEX);
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpfnWndProc = (WNDPROC)WndProc;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hInstance = _instance;
    winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winClass.lpszMenuName = NULL;
    winClass.lpszClassName = _title;
    winClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    if (!RegisterClassEx(&winClass))
    {
        std::cout << "Unexpected error creating the window!" << std::endl;
        fflush(stdout);
        exit(1);
    }

    auto posX = 100;
    auto posY = 100;
    auto windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU;
    HWND parentHandle = nullptr;
    HMENU menuHandle = nullptr;
    LPVOID extraParams = nullptr;
    RECT windowRectangle = { 0, 0, _width, _height };

    AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
    auto actualWidth = windowRectangle.right - windowRectangle.left;
    auto actualHeight = windowRectangle.bottom - windowRectangle.top;

    _handle = CreateWindowEx(0,
        _title,
        _title,
        windowStyle,
        posX,
        posY,
        actualWidth,
        actualHeight,
        parentHandle,
        menuHandle,
        _instance,
        extraParams);

    if (!_handle)
    {
        std::cout << "Error opening the window!" << std::endl;
        fflush(stdout);
        exit(1);
    }
}

window::~window()
{
    DestroyWindow(_handle);
}

void window::run()
{
    while (true)
    {
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
}