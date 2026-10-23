#pragma once

#include "containerBox.hpp"
#include "textContainer.hpp"

class TextField: public ContainerBox{

private:
    float padding;
    ContainerBox* paddingContainer;
    ContainerBox* fieldContainer;
    std::vector<TextContainer*> textContainers; 
    std::vector<std::string> wordVector;
    std::vector<float> wordWidths;
    float absFontSize;
    std::pair<AlignmentX, AlignmentY> textAlignment;
    bool doAutoWidth;

    std::vector<std::string> string2WordVector(std::string text);
    std::vector<std::string> wordVector2LineVector(std::vector<std::string> wordVector);
    void updateTextConatiners();
    void updateFieldContainerHeight();

public:
    TextField(Vector2 fieldPosition, float fieldWidth, float fieldHeight, float absFontSize, std::string text = "", float padding = 0.0f,
        std::pair<AlignmentX, AlignmentY> textAlignment = {AlignmentX::LEFT, AlignmentY::TOP}, bool doAutoWidth = false);
    ~TextField();

    void setText(std::string newText);
    std::string getText();
    void setTextAlignment(std::pair<AlignmentX, AlignmentY> newTextAlignment);

    virtual void Draw();
    void drawDebug(Color colour = GREEN) override;

    void setSize(float newDecimalWidth, float newDecimalHeight, bool isSilent = false) override;
    void setPadding(float newPadding);
    void onScreenResize() override;
    void onAnyResizeComplete(ContainerBox* source) override;
};