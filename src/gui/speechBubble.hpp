#pragma once

#include "textField.hpp"
#include "textContainer.hpp"
#include "timer.hpp"
#include "uiElement.hpp"

class SpeechBubble: public TextField{
private:
    ContainerBox* stemTip;
    float stemWidthPercentage;
    Timer animTimer;
    bool doAutoWidth;
    std::vector<UIElement*> controls;
public:
    SpeechBubble(Vector2 stemTipOffset, Vector2 bubblePosition, std::string text, float stemWidthPercentage, 
        bool doAutoWidth = true, float BubbleWidth = 0.0f, float BubbleHeight = 0.0f);
    ~SpeechBubble();
    void hide();
    void show();
    void Draw() override;

    UIElement* addControl(std::unique_ptr<UIElement> control);

    void setStemTipPosition(Vector2 newPosition);

};