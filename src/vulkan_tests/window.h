#pragma once

#include "mouseEventArgs.h"

#include <Windows.h>
#include <string>
#include <functional>

class window
{
protected:
    HWND _handle;
    HINSTANCE _instance;
    LPCSTR _title;
    int _width;
    int _height;
    bool _shouldExit;
private:
    void openWindow();
    void dispatchInput();
protected:
    void handleMessages();
    virtual void onLoaded() {};
    virtual void loop() = 0;
    virtual void onMouseMove(mouseEventArgs* eventArgs) {};
    virtual void onMouseUp(mouseEventArgs* eventArgs) {};
    virtual void onMouseDown(mouseEventArgs* eventArgs) {};
public:
    window(int width, int height, char* title);
    ~window();

    void show();

    inline HWND getHandle() const { return _handle; }
    inline HINSTANCE getInstance() const { return _instance; }
    inline int getWidth() const { return _width; }
    inline int getHeight() const { return _height; }

    inline void setWidth(int value) { _width = value; }
    inline void setHeight(int value) { _height = value; }
};

