#include "window.h"

#include "mouseEventArgs.h"

#include <iostream>

enum eventType
{
    none,
    onMouseMove,
    OnMouseDown,
    onMouseUp,
};

mouseEventArgs* _mouseEventArgs;
eventType _eventType = eventType::none;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    POINT mousePos;
    auto mouseButton = mouseEventArgs::mouseButtons::none;

    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        break;
    case WM_KEYDOWN:
        break;
    case WM_KEYUP:
        break;
    case WM_RBUTTONDOWN:
        mouseButton = mouseEventArgs::mouseButtons::right;
        _eventType = eventType::OnMouseDown;
        break;
    case WM_LBUTTONDOWN:
        mouseButton = mouseEventArgs::mouseButtons::left;
        _eventType = eventType::OnMouseDown;
        break;
    case WM_MBUTTONDOWN:
        mouseButton = mouseEventArgs::mouseButtons::middle;
        _eventType = eventType::OnMouseDown;
        break;
    case WM_RBUTTONUP:
        mouseButton = mouseEventArgs::mouseButtons::right;
        _eventType = eventType::onMouseUp;
        break;
    case WM_LBUTTONUP:
        mouseButton = mouseEventArgs::mouseButtons::left;
        _eventType = eventType::onMouseUp;
        break;
    case WM_MBUTTONUP:
        mouseButton = mouseEventArgs::mouseButtons::middle;
        _eventType = eventType::onMouseUp;
        break;
    case WM_MOUSEMOVE:
        _eventType = eventType::onMouseMove;
        break;
    default:
        break;
    }

    GetCursorPos(&mousePos);
    ScreenToClient(hWnd, &mousePos);
    _mouseEventArgs = new mouseEventArgs(mouseButton, glm::vec2(mousePos.x, mousePos.y));

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

window::window(int width, int height, char* title) :
    _instance(GetModuleHandle(NULL)),
    _title(title),
    _width(width),
    _height(height),
    _shouldExit(false)
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
}

window::~window()
{
    DestroyWindow(_handle);
}

void window::show()
{
    openWindow();
    onLoaded();
    loop();
}

void window::openWindow()
{
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

void window::handleMessages()
{
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            _shouldExit = true;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            dispatchInput();
        }
    }
}

void window::dispatchInput()
{
    if (_mouseEventArgs == nullptr)
        return;

    switch (_eventType)
    {
    case eventType::onMouseMove:
        onMouseMove(_mouseEventArgs);
        break;
    case eventType::OnMouseDown:
        onMouseDown(_mouseEventArgs);
        break;
    case eventType::onMouseUp:
        onMouseUp(_mouseEventArgs);
        break;
    case eventType::none:
        break;
    default:
        break;
    }
}