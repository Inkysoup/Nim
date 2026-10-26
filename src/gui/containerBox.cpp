#include "containerBox.hpp"

#include <algorithm>
#include <memory>

#include <iostream>



std::set<ContainerBox*> ContainerBox::allContainers;
std::set<ContainerBox*> ContainerBox::resizeListeners;

ContainerBox::ContainerBox(Vector2 decimalAcrossParent, float boxDecimalWidth, float boxDecimalHeight)
    : parent(nullptr), originAlignment{LEFT, TOP}, originOffset{0.0f, 0.0f}{
    allContainers.insert(this);

    ContainerBox::screenMagnitude = getScreenMagnitude();
    this->boxDecimalWidth = boxDecimalWidth;
    this->boxDecimalHeight = boxDecimalHeight;

    this->decimalAcrossParent = decimalAcrossParent;
    this->setSize(boxDecimalWidth, boxDecimalHeight);

    this->setOffset(decimalAcrossParent);

    this->setOriginAlignment({ContainerBox::LEFT, ContainerBox::TOP});
}

ContainerBox::~ContainerBox(){
    if(resizeListeners.contains(this)){
        removeResizeListener(this);
    }
    allContainers.erase(this);
}

const std::set<ContainerBox *> &ContainerBox::getAllContainers(){
    return allContainers;
}

void ContainerBox::setScreenRatio(float newRatio){
    widthHeightRatio = newRatio;
    for(ContainerBox* container : ContainerBox::getAllContainers()){
        container->onScreenResize();
    }
}

float ContainerBox::getScreenMagnitude(){
    return std::min(GetScreenWidth()*1.0f, GetScreenHeight()*0.8f);
}

Vector2 ContainerBox::getScreenSize(){
    return {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
}

void ContainerBox::drawDebug(Color colour){
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, colour);
    //DrawRectangleLinesEx(rect, 2.0f, colour);
}

Rectangle ContainerBox::getRect() const{
    return rect;
}

ContainerBox* ContainerBox::addChild(std::unique_ptr<ContainerBox> child){
    // set parent pointer for the child and store ownership
    child->parent = this;
    child->updateOffsets();
    children.emplace_back(std::move(child));
    return children.back().get();
}

void ContainerBox::clearChildren(){
    children.clear();
}

std::vector<ContainerBox*> ContainerBox::getChildren() const {
    std::vector<ContainerBox*> returnChildren;
    returnChildren.reserve(children.size());
    for (const auto &c : children) returnChildren.push_back(c.get());
    return returnChildren;
}

ContainerBox* ContainerBox::getParent() const{
    return parent;
}

void ContainerBox::setSize(float newDecimalWidth, float newDecimalHeight, bool isSilent){
    boxDecimalWidth = newDecimalWidth;
    boxDecimalHeight = newDecimalHeight;
    rect = {0, 0, newDecimalWidth*screenMagnitude, newDecimalHeight*screenMagnitude};
    updatePosition(isSilent);
}

Vector2 ContainerBox::getSize(){
    return Vector2{rect.width/screenMagnitude, rect.height/screenMagnitude};
}

float ContainerBox::getOutlineWidth(){
    return outlineWidth;
}

void ContainerBox::updateOriginOffset(){
    if(originAlignment.first == LEFT){
        originOffset.x = 0.0f;
    }
    else if(originAlignment.first == CENTRE_X){
        originOffset.x = rect.width/2.0f;
    }
    else if(originAlignment.first == RIGHT){
        originOffset.x = rect.width;
    }

    if(originAlignment.second == TOP){
        originOffset.y = 0.0f;
    }
    else if(originAlignment.second == CENTRE_Y){
        originOffset.y = rect.height/2.0f;
    }
    else if(originAlignment.second == BOTTOM){
        originOffset.y = rect.height;
    }
}
void ContainerBox::updatePosition(bool isSilent){
    updateOriginOffset();

    Rectangle parentRect;
    if(parent == nullptr){
        parentRect = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    }
    else{
        parentRect = (parent)->getRect();
    }

    rect.x = parentRect.x + decimalAcrossParent.x * parentRect.width - originOffset.x;
    rect.y = parentRect.y + decimalAcrossParent.y * parentRect.height - originOffset.y;
    if(!isSilent){
        for(auto& child : children){
            child->updatePosition();
        }
    }
}

void ContainerBox::setOffsetX(float decimalAcrossParent){
    this->decimalAcrossParent.x = decimalAcrossParent;
    updatePosition();
}
void ContainerBox::setOffsetY(float decimalAcrossParent){
    this->decimalAcrossParent.y = decimalAcrossParent;
    updatePosition();
}
void ContainerBox::setOffset(Vector2 decimalAcrossParent){
    this->decimalAcrossParent = decimalAcrossParent;
    updatePosition();
}
void ContainerBox::updateOffsets(){
    updatePosition();
    for(auto& child : children){
        child->updateOffsets();
    }
}
void ContainerBox::setOriginAlignment(AlignmentX newAlignment){
    originAlignment.first = newAlignment;
    updatePosition();
}
void ContainerBox::setOriginAlignment(AlignmentY newAlignment){
    originAlignment.second = newAlignment;
    updatePosition();
}
void ContainerBox::setOriginAlignment(std::pair<AlignmentX, AlignmentY> newAlignmentXY){
    originAlignment = newAlignmentXY;
    updatePosition();
}

void ContainerBox::onScreenResize(){
    rect = {0, 0, boxDecimalWidth*screenMagnitude, boxDecimalHeight*screenMagnitude};
    updatePosition();
    
    for(ContainerBox* listener : resizeListeners) {
        listener->onAnyResizeComplete(this);
    }
}
void ContainerBox::onScreenResizeManager(){
    ContainerBox::screenMagnitude = ContainerBox::getScreenMagnitude();
    ContainerBox::outlineWidth = screenMagnitude*0.005;
    for(ContainerBox* container : ContainerBox::getAllContainers()){
        container->onScreenResize();
    }
}

void ContainerBox::onAnyResizeComplete(ContainerBox *source){
    return;
}

void ContainerBox::addResizeListener(ContainerBox *listener){
    resizeListeners.insert(listener);
}

void ContainerBox::removeResizeListener(ContainerBox *listener){
    resizeListeners.erase(listener);
}

