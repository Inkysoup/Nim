#include "textButton.hpp"
#include "constants.hpp"

using namespace constants;

TextButton::TextButton(Vector2 buttonPosition, float buttonWidth, float buttonHeight, Color buttonColor, std::string buttonText)
        : Button(buttonPosition, buttonWidth, buttonHeight, buttonColor){

    {
        auto tc = std::make_unique<TextContainer>(Vector2{0.5f, 0.5f}, buttonPadding, buttonText, 24.0f,
            std::pair<ContainerBox::AlignmentX, ContainerBox::AlignmentY>{ContainerBox::CENTRE_X, ContainerBox::CENTRE_Y}, BLACK);
        textContainer = static_cast<TextContainer*>(topFaceContainer->addChild(std::move(tc)));
    }
}

TextButton::~TextButton(){
}

void TextButton::Draw(){
    if(getIsDisabled()) return;
    Button::Draw();

    textContainer->setColour(BLACK);
    if(buttonState == HELD or pressedTimer.getSecondsRemaining() > 0.0f){
        textContainer->setColour(WHITE);
    }
    else if(buttonState==HOVERED){
        textContainer->setColour(WHITE);
    }
    textContainer->Draw();
}

void TextButton::setText(std::string newText){
    textContainer->setText(newText);
}

void TextButton::setAbsFontSize(float newAbsFontSize){
    textContainer->setAbsFontSize(newAbsFontSize);
}
