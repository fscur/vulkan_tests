#pragma once

#include "window.h"
#include "renderer.h"
#include "camera.h"
#include "mouseEventArgs.h"

class screen : public window
{
private:
    camera* _camera;
    renderer* _renderer;
    glm::vec2 _lastMousePosition;
    bool _rotating;
    bool _panning;
public:
    screen(int width, int height, char* title);
    ~screen() { window::~window(); };

    void onLoaded() override;
    void onMouseUp(mouseEventArgs* eventArgs) override;
    void onMouseDown(mouseEventArgs* eventArgs) override;
    void onMouseMove(mouseEventArgs* eventArgs) override;
    void loop() override;
    void update();
    void render();
};

