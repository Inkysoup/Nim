#pragma once

#include "uiElement.hpp"

class SelectionSet: public UIElement{
private:
    std::vector<ContainerBox*> members;
public:
    SelectionSet();
    ~SelectionSet();

    Rectangle getBoundingBox();

    ContainerBox* addChild(std::unique_ptr<ContainerBox> child) = delete;
    std::vector<ContainerBox*> getChildren() = delete;
    void addMember(ContainerBox* member);
    void removeMember(ContainerBox* member);
    std::vector<ContainerBox*> getMembers();

    void resizeBoundingBox();

    void handleKeyboardInput(int keycode) override;
    bool isPressed(Vector2 mousePos, bool mouseUp, bool mouseDown) override;
    void onScreenResize() override;

};