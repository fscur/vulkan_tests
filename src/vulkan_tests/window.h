#pragma once

#include <Windows.h>
#include <string>
#include <functional>

class window
{
private:
    HWND _handle;
    HINSTANCE _instance;
    LPCSTR _title;
    int _width;
    int _height;
    std::function<void(void)> _drawCallback = [] {};
public:
    window(int width, int height, char* title);
    ~window();

    void run();

    inline HWND getHandle() const { return _handle; }
    inline HINSTANCE getInstance() const { return _instance; }
    inline int getWidth() const { return _width; }
    inline int getHeight() const { return _height; }

    inline void setWidth(int value) { _width = value; }
    inline void setHeight(int value) { _height = value; }

    inline void setDrawCallback(std::function<void(void)> callback) { _drawCallback = callback; }
};

