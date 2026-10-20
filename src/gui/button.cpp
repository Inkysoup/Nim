#include "button.hpp"
#include "constants.hpp"
using namespace constants;

Button::Button(Vector2 buttonPosition, float buttonWidth, float buttonHeight, Color buttonColor)
        : UIElement(buttonPosition, buttonWidth, buttonHeight), pressedTimer(0.0f){

    {
        auto tfc = std::make_unique<ContainerBox>(Vector2{0.5f, 0.0f}, buttonWidth, buttonHeight - buttonDepth);
        tfc->setOriginAlignment({ContainerBox::CENTRE_X, ContainerBox::TOP});
        topFaceContainer = static_cast<ContainerBox*>(UIElement::addChild(std::move(tfc)));
    }

    buttonState = RELEASED;
    colour = buttonColor;
}

Button::~Button(){
}

void Button::Draw(){
    if(buttonState == HELD or pressedTimer.getSecondsRemaining() > 0.0f){
        topFaceContainer->setOriginAlignment(ContainerBox::BOTTOM);
        topFaceContainer->setOffsetY(1.0f);
        Rectangle topFaceRect = topFaceContainer->getRect();
        DrawRectangleRounded(topFaceRect, 0.5f, 5, colour);
        DrawRectangleRounded(topFaceRect, 0.5f, 5, darkenOverlay);
        DrawRectangleRoundedLinesEx(topFaceRect, 0.5f, 5, 2, BLACK);
    }
    else{
        Rectangle rect = UIElement::getRect();
        topFaceContainer->setOriginAlignment(ContainerBox::TOP);
        topFaceContainer->setOffsetY(0.0f);
        Rectangle topFaceRect = topFaceContainer->getRect();
        DrawRectangleRounded(rect, 0.5f, 5, colour); // side face
        DrawRectangleRounded(rect, 0.5f, 5, darkenOverlay);
        DrawRectangleRounded(topFaceRect, 0.5, 5, colour); // top face
        if(buttonState==HOVERED){
            float hoveredRectInset = getOutlineWidth();
            float hoveredRectWidth = std::max(0.0f, topFaceRect.width-hoveredRectInset*2.0f);
            float hoveredRectHeight = std::max(0.0f, topFaceRect.height-hoveredRectInset*2.0f - buttonDepth);
            Rectangle hoveredRect = {topFaceRect.x + hoveredRectInset, topFaceRect.y + hoveredRectInset, hoveredRectWidth, hoveredRectHeight};
            DrawRectangleRounded(hoveredRect, 0.5f, 5, darkenOverlay);
        }
        DrawRectangleRoundedLinesEx(rect, 0.5f, 5, getOutlineWidth(), BLACK);
    }
}

void Button::handleKeyboardInput(int keycode){
    if(keycode == KEY_ENTER){
        buttonState = RELEASED;
        onClickFunc();
        pressedTimer.setSecondsRemaining(0.1f);
        pressedTimer.resume();
    }
}

void Button::setColour(Color newColour){
    colour = newColour;
}
