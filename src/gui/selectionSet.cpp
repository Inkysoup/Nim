#include "selectionSet.hpp"
#include <algorithm>


SelectionSet::SelectionSet():
        UIElement({0.0f, 0.0f}, 0.0f, 0.0f){
}

SelectionSet::~SelectionSet(){
}

Rectangle SelectionSet::getBoundingBox(){
    auto members = getMembers();
    for (auto it = members.begin(); it != members.end();){ // if member is disabled, don't include it in bounding box.
        if(auto* uiElement = dynamic_cast<UIElement*>(*it)){
            if(uiElement->getIsDisabled()){
                it = members.erase(it);
                continue;
            }
        }
        ++it;
    }
    if(members.empty()){
        return Rectangle{0.0f,0.0f,0.0f,0.0f};
    }

    Rectangle firstRect = members[0]->getRect();
    Vector2 topRight = {firstRect.x, firstRect.y};
    Vector2 bottomLeft = {firstRect.x + firstRect.width, firstRect.y + firstRect.height};
    for(ContainerBox* member: members){
        Rectangle rect = member->getRect();
        if(rect.x < topRight.x) topRight.x = rect.x;
        if(rect.y < topRight.y) topRight.y = rect.y;
        if(rect.x + rect.width > bottomLeft.x) bottomLeft.x = rect.x + rect.width;
        if(rect.y + rect.height > bottomLeft.y) bottomLeft.y = rect.y + rect.height;
    }
    Rectangle boundingBox = {topRight.x, topRight.y, bottomLeft.x - topRight.x, bottomLeft.y - topRight.y};
    return boundingBox;
}

void SelectionSet::addMember(ContainerBox* member){
    members.push_back(member);
}

void SelectionSet::removeMember(ContainerBox* member){ // TODO: ensure this is called by members in destructor 
    auto it = std::find(members.begin(), members.end(), member);
    if(it != members.end()){
        members.erase(it);
    }
}

std::vector<ContainerBox*> SelectionSet::getMembers(){
    return members;
}

void SelectionSet::resizeBoundingBox(){
    Rectangle newAbsRect = getBoundingBox();
    float screenMag = getScreenMagnitude();
    float newX = newAbsRect.x/GetScreenWidth();
    float newY = newAbsRect.y/GetScreenHeight();
    float newWidth = newAbsRect.width/screenMag;
    float newHeight = newAbsRect.height/screenMag;
    ContainerBox::setOffset({newX, newY});
    ContainerBox::setSize(newWidth, newHeight);
    if(newWidth == 0.0f and newHeight == 0.0f) setTabNavigatable(false);
}

void SelectionSet::handleKeyboardInput(int keycode){
    return;
}

bool SelectionSet::isPressed(Vector2 mousePos, bool mouseUp, bool mouseDown){
    return false;
}

void SelectionSet::onScreenResize()
{
    for(ContainerBox* member: getMembers()){
        member->onScreenResize();
    }
    resizeBoundingBox();
}
