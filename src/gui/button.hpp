#pragma once
#include <raylib.h>
#include <string>
#include "uiElement.hpp"
#include "timer.hpp"

class Button: public UIElement{
protected:
    ContainerBox* topFaceContainer;
    Color colour;
    Timer pressedTimer;
    void Draw();
public:
    Button(Vector2 buttonPosition, float buttonWidth, float buttonHeight, Color buttonColor);
    ~Button();
    void handleKeyboardInput(int keycode);
    void setColour(Color newColour);
};