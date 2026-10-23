#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include "containerBox.hpp"

class TextContainer: public ContainerBox{
private:
    struct FontResources { Font font; Shader shader; };
    static std::shared_ptr<FontResources> fontResources;
    static Font loadFontSDF(std::string asciiFont, std::vector<std::pair<std::string, std::vector<int>>> fontCodepoints = {});

    float padding;
    std::string text;
    Color colour;
    float alpha;
    std::vector<std::pair<float, Color>> outlines;
    
    float absFontSize;
    float fontSize;
    float fontSpacing;

    void resizeContainerBox();
public:
    TextContainer(Vector2 containerPosition, float padding, std::string text, 
        float fontSize, std::pair<AlignmentX, AlignmentY> alignment = {CENTRE_X, CENTRE_Y},
        Color textColour = BLACK, float textAlpha = 1.0f);
    ~TextContainer();

    virtual void Draw();

    void setText(std::string newText);
    std::string getText();

    static Font* getSDFFont();
    void setAbsFontSize(float newAbsFontSize);
    float getAbsFontSize();
    float getFontSize();
    float getFontSpacing();
    static float calculateTrueFontSize(float absFontSize);
    static float calculateFontSpacing(float trueFontSize);
    Vector2 getTextSize();
    void setPadding(float newPadding);
    void setColour(Color newColour);
    Color getColour();
    void setAlpha(float newAlpha);

    void appendOutline(float outlineFract, Color colour);
    void removeOutline(unsigned index);

    void onScreenResize() override;
};