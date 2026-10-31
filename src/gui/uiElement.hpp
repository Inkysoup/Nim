#pragma once
#include <raylib.h>
#include <set>
#include <functional>

#include "containerBox.hpp"


class UIElement: public ContainerBox{
private:
    static std::vector<UIElement*> allElements;
    bool isDisabled;
    bool doAutoDraw;
    bool isTabNavigatable;
    bool allowedActive;
    static UIElement* ptrActive;
    static std::pair<float, Color> activeLineProperties[2];
    static void sortAllElements();
public:
    enum State{
        RELEASED,
        HOVERED,
        HELD,
    };
protected:
    std::function<void(void)> onClickFunc;
    std::function<void(void)> onUnactiveFunc;
    State buttonState;
    Vector2 visualOffset;
    void callUnactiveFunc();
public:
    UIElement(Vector2 boxPosition, float boxWidth, float boxHeight);
    virtual ~UIElement();

    static const std::vector<UIElement*> &getAllElements();

    virtual bool isPressed(Vector2 mousePos, bool mouseUp, bool mouseDown);
    void setPressed();
    void setActive();
    void setAllowedActive(bool allowedActive);
    static UIElement* getActive();
    bool isActive();
    static void clearActive();
    static void progressActive(int indexes);
    static void drawActive();
    void drawSelect();
    void setTabNavigatable(bool isTabNavigatable);
    bool getIsTabNavigatable();
    virtual void Draw();
    virtual void handleKeyboardInput(int keycode) = 0;
    void bindOnClick(std::function<void(void)> func);
    void bindOnUnactive(std::function<void(void)> func);
    UIElement::State getButtonState();
    void setButtonState(UIElement::State newState);
    
    void disable();
    void enable();
    bool getIsDisabled();
    void setAutoDraw(bool doAutoDraw);
    bool getIsAutoDraw();

    void setVisualOffset(Vector2 newVisualOffset); // TODO: apply to all elements, not just inputbox
};