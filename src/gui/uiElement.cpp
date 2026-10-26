#include "uiElement.hpp"
#include <rlgl.h>
#include "constants.hpp"
#include <algorithm>
#include <iostream>
using namespace constants;

std::vector<UIElement*> UIElement::allElements;
UIElement* UIElement::ptrActive = nullptr;
std::pair<float, Color> UIElement::activeLineProperties[2];

void UIElement::sortAllElements(){
    std::sort(allElements.begin(), allElements.end(), [](UIElement* a, UIElement* b) {
        auto posA = a->getRect();
        auto posB = b->getRect();
        
        if (posA.y != posB.y) {
            return posA.y < posB.y;
        }
        return posA.x < posB.x;
    });
}

void UIElement::callUnactiveFunc(){
    onUnactiveFunc();
}

UIElement::UIElement(Vector2 boxPosition, float boxWidth, float boxHeight) : ContainerBox{boxPosition, boxWidth, boxHeight}{
    allElements.push_back(this);
    visualOffset = {0.0f, 0.0f};
    isDisabled = false;
    doAutoDraw = true;
    isTabNavigatable = true;
    onClickFunc = [](){return;};
    onUnactiveFunc = [](){return;};
}

UIElement::~UIElement(){
    if(ptrActive == this){
        ptrActive->callUnactiveFunc();
        ptrActive = nullptr;
    }
    allElements.erase(std::remove(allElements.begin(), allElements.end(), this), allElements.end());
}


const std::vector<UIElement*> &UIElement::getAllElements(){
    return allElements;
}

void UIElement::bindOnClick(std::function<void(void)> func){
    onClickFunc = func;
}

void UIElement::bindOnUnactive(std::function<void(void)> func){
    onUnactiveFunc = func;
}


UIElement::State UIElement::getButtonState(){
    return buttonState;
}

void UIElement::setButtonState(UIElement::State newState){
    buttonState = newState;
}

void UIElement::disable(){
    isDisabled = true;
    allElements.erase(std::remove(allElements.begin(), allElements.end(), this), allElements.end());
}

void UIElement::enable(){
    isDisabled = false;
    allElements.push_back(this);
    
}

bool UIElement::getIsDisabled(){
    return isDisabled;
}

void UIElement::setAutoDraw(bool doAutoDraw){
    this->doAutoDraw = doAutoDraw;
}

bool UIElement::getIsAutoDraw(){
    return doAutoDraw;
}

void UIElement::setVisualOffset(Vector2 newVisualOffset){
    visualOffset = newVisualOffset;
}

void UIElement::setPressed(){
    buttonState = RELEASED;
}

void UIElement::setActive(){
    if(ptrActive != nullptr){
        ptrActive->callUnactiveFunc();
    }
    ptrActive = this;
}
UIElement* UIElement::getActive(){
    return ptrActive;
}
bool UIElement::isActive(){
    return ptrActive == this ? true : false;
}
void UIElement::clearActive(){
    if(ptrActive != nullptr){
        ptrActive->callUnactiveFunc();
    }
    ptrActive = nullptr;
}
void UIElement::progressActive(int indexes){ // only valid for -1, 0, 1 if any element isnt tabnavigatable.
    sortAllElements();

    if(allElements.empty()) {
        ptrActive = nullptr;
        return;
    }
    
    auto it = std::find(allElements.begin(), allElements.end(), ptrActive);
    if(it == allElements.end()){
        ptrActive = allElements[0];
        return;
    }
    int initialIndex = std::distance(std::begin(allElements), it);
    
    int i = std::distance(allElements.begin(), it);
    int newIndex = (i+indexes + allElements.size())%allElements.size(); // (a%b + b)%b) to handle negatives
    int progressionDir = indexes < 0 ? -1 : 1;
    while(!allElements[newIndex]->getIsTabNavigatable()){
        newIndex = (newIndex+progressionDir + allElements.size())%allElements.size();
        if(newIndex == initialIndex){
            ptrActive = nullptr;
            return;
        }
    }
    ptrActive = allElements[newIndex];
}
void UIElement::drawActive(){
    if(ptrActive != nullptr){
        ptrActive->drawSelect();
    }
}

void UIElement::drawSelect(){
    float thickness = getOutlineWidth();
    float offset = 2.0f*thickness;
    float cornerLength = 5.0f*thickness;
    
    Rectangle rect = getRect();
    rlPushMatrix();
    rlTranslatef(rect.x, rect.y, 0.0f);
        // --- dashed rect --- //
    //     DrawLineEx({-offset, -offset}, {rect.width+offset, -offset}, 3, BLACK);
    //     DrawLineEx({rect.width + offset, -offset}, {rect.width + offset, rect.height + offset}, 3, BLACK);
    //     DrawLineEx({rect.width + offset, rect.height + offset}, {-offset, rect.height + offset}, 3, BLACK);
    //     DrawLineEx({-offset, rect.height + offset}, {-offset, -offset}, 3, BLACK);

    //     DrawLineDashed({-offset, -offset}, {rect.width+offset, -offset}, 5, 5, WHITE);
    //     DrawLineDashed({rect.width + offset, -offset}, {rect.width + offset, rect.height + offset}, 5, 5, WHITE);
    //     DrawLineDashed({rect.width + offset, rect.height + offset}, {-offset, rect.height + offset}, 5, 5, WHITE);
    //     DrawLineDashed({-offset, rect.height + offset}, {-offset, -offset}, 5, 5, WHITE);
        // ------------------- //

        // --- corners --- //
        activeLineProperties[0] = {thickness*3.0f, BLACK};
        activeLineProperties[1] = {thickness, WHITE};
        for(auto properties: activeLineProperties){
            DrawLineEx({-offset, -offset}, {cornerLength - offset, -offset}, properties.first, properties.second);
            DrawLineEx({-offset, -offset}, {-offset, cornerLength-offset,}, properties.first, properties.second);
            DrawLineEx({rect.width + offset, -offset}, {rect.width + offset - cornerLength, -offset}, properties.first, properties.second);
            DrawLineEx({rect.width + offset, -offset}, {rect.width + offset, cornerLength - offset}, properties.first, properties.second);
            DrawLineEx({-offset, rect.height + offset}, {-offset, rect.height + offset - cornerLength}, properties.first, properties.second);
            DrawLineEx({-offset, rect.height + offset}, {cornerLength - offset, rect.height + offset}, properties.first, properties.second);
            DrawLineEx({rect.width + offset, rect.height + offset}, {rect.width + offset, rect.height + offset - cornerLength}, properties.first, properties.second);
            DrawLineEx({rect.width + offset, rect.height + offset}, {rect.width + offset - cornerLength, rect.height + offset}, properties.first, properties.second);
            DrawCircle(-offset, -offset, properties.first/2.0f, properties.second);
            DrawCircle(cornerLength - offset, -offset, properties.first/2.0f, properties.second);
            DrawCircle(-offset, cornerLength-offset, properties.first/2.0f, properties.second);
            DrawCircle(rect.width + offset, -offset, properties.first/2.0f, properties.second);
            DrawCircle(rect.width + offset - cornerLength, -offset, properties.first/2.0f, properties.second);
            DrawCircle(rect.width + offset, cornerLength - offset, properties.first/2.0f, properties.second);
            DrawCircle(-offset, rect.height + offset, properties.first/2.0f, properties.second);
            DrawCircle(-offset, rect.height + offset - cornerLength, properties.first/2.0f, properties.second);
            DrawCircle(cornerLength - offset, rect.height + offset, properties.first/2.0f, properties.second);
            DrawCircle(rect.width + offset, rect.height + offset, properties.first/2.0f, properties.second);
            DrawCircle(rect.width + offset, rect.height + offset - cornerLength, properties.first/2.0f, properties.second);
            DrawCircle(rect.width + offset - cornerLength, rect.height + offset, properties.first/2.0f, properties.second);
        }
        // --------------- //
    rlPopMatrix();
}

void UIElement::setTabNavigatable(bool isTabNavigatable){
    this->isTabNavigatable = isTabNavigatable;
}

bool UIElement::getIsTabNavigatable(){
    return isTabNavigatable;
}

void UIElement::Draw(){
}

bool UIElement::isPressed(Vector2 mousePos, bool mouseUp, bool mouseDown)
{
    if(isDisabled){
        buttonState = RELEASED;
        return false;
    }
    if(CheckCollisionPointRec(mousePos, ContainerBox::getRect())){
        if(mouseDown){
            buttonState = HELD;
        }
        else{
            buttonState = HOVERED;
        }
    }
    else{
        buttonState = RELEASED;
    }

    if(CheckCollisionPointRec(mousePos, ContainerBox::getRect()) and mouseUp){
        setActive();
        onClickFunc();
        buttonState = HOVERED;
        return true;
    } 
    return false;
}

