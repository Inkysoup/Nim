#pragma once
#include <raylib.h>
#include <set>
#include <vector>
#include <memory>



class ContainerBox{
public:
    enum AlignmentX{
        LEFT,
        CENTRE_X,
        RIGHT,
    };
    enum AlignmentY{
        TOP,
        CENTRE_Y,
        BOTTOM,
    };
private:
    static std::set<ContainerBox*> allContainers;
    static std::set<ContainerBox*> resizeListeners;

    std::vector<std::unique_ptr<ContainerBox>> children;
    ContainerBox* parent;
    std::pair<AlignmentX, AlignmentY> originAlignment;
    Vector2 originOffset;
    Rectangle rect;
    float boxDecimalWidth;
    float boxDecimalHeight;
    Vector2 decimalAcrossParent;
    
    static inline float widthHeightRatio;
    static inline float outlineWidth;
    static inline int screenMagnitude;

    void updateOriginOffset();
    void updatePosition(bool isSilent = false);
public:
    ContainerBox(Vector2 decimalAcrossParent, float boxDecimalWidth, float boxDecimalHeight);
    ~ContainerBox();

    static const std::set<ContainerBox*> &getAllContainers();

    static void setScreenRatio(float newRatio);
    static float getScreenMagnitude();
    static Vector2 getScreenSize();
    
    virtual void drawDebug(Color colour = MAGENTA);

    Rectangle getRect() const;
    ContainerBox* addChild(std::unique_ptr<ContainerBox> child);
    void clearChildren();
    std::vector<ContainerBox*> getChildren() const;
    
    virtual void setSize(float newDecimalWidth, float newDecimalHeight, bool isSilent = false);
    Vector2 getSize();

    static float getOutlineWidth();

    void setOffsetX(float decimalAcrossParent);
    void setOffsetY(float decimalAcrossParent);
    void setOffset(Vector2 decimalAcrossParent);
    void updateOffsets();
    
    void setOriginAlignment(AlignmentX newAlignmentX);
    void setOriginAlignment(AlignmentY newAlignmentY);
    void setOriginAlignment(std::pair<AlignmentX, AlignmentY> newAlignmentXY);
    
    virtual void onScreenResize();
    static void onScreenResizeManager();
    virtual void onAnyResizeComplete(ContainerBox* source);
    // should remove below and usage in textfield in favour of making parents trigger their childrens' onscreenresize method
    void addResizeListener(ContainerBox* listener); 
    void removeResizeListener(ContainerBox* listener);
};

