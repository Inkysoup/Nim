#pragma once

#include "button.hpp"
#include "textContainer.hpp"

class TextButton: public Button{
private:
    TextContainer* textContainer;
public:
    TextButton(Vector2 buttonPosition, float buttonWidth, float buttonHeight, Color buttonColor, std::string buttonText);
    ~TextButton();
    void Draw();
    void setText(std::string newText);
    void setAbsFontSize(float newAbsFontSize);
};