#include "textField.hpp"
#include <sstream>
#include <iostream>

std::vector<std::string> TextField::string2WordVector(std::string text){
    std::vector<std::string> words;

    std::string line = "";
    std::stringstream ssLines(text);
    while(std::getline(ssLines, line)){
        if(!words.empty()){
            words.push_back("\n");
        }
        std::string word;
        std::stringstream ssWords(line);
        while (ssWords >> word) {
            words.push_back(word);
        }
    }
    return words;
}

std::vector<std::string> TextField::wordVector2LineVector(std::vector<std::string> wordVector){
    Rectangle rect = fieldContainer->getRect();
    Font* font = TextContainer::getSDFFont();
    float trueFontSize = TextContainer::calculateTrueFontSize(absFontSize);
    float fontSpacing = TextContainer::calculateFontSpacing(trueFontSize);
    float spaceWidth = MeasureTextEx(*font, " ", trueFontSize, fontSpacing).x + fontSpacing*2.0f;

    std::vector<std::string> lineVector;
    lineVector.push_back("");
    lineVector.reserve(wordVector.size());
    
    std::string currentLine;
    float currentLineLength = 0.0f;
    for(unsigned i = 0; i < wordVector.size(); ++i){
        if(wordVector[i] == "\n"){
            lineVector.push_back("");
            currentLineLength = 0;
        }
        else if(currentLineLength + wordWidths[i] + spaceWidth > rect.width and !doAutoWidth){
            if(lineVector.back().empty()){
                lineVector.back() += wordVector[i];
            }
            lineVector.push_back("");
            currentLineLength = 0;
        }  
        else{
            if(!lineVector.back().empty()){
                lineVector.back() += " ";
                currentLineLength += spaceWidth;
            }
            lineVector.back() += wordVector[i];
            currentLineLength += wordWidths[i];
            if(currentLineLength > rect.width){
                std::cout<< currentLineLength << " " << rect.width << "\n";
            }
        }   
    }
    return lineVector;
}

void TextField::updateTextConatiners(){
    fieldContainer->setSize(ContainerBox::getSize().x-2.0f*padding, fieldContainer->getSize().y);
    wordWidths.clear();
    Font* font = TextContainer::getSDFFont();
    float trueFontSize = TextContainer::calculateTrueFontSize(absFontSize);
    float fontSpacing = TextContainer::calculateFontSpacing(trueFontSize);
    for(std::string word: wordVector){
        wordWidths.push_back(MeasureTextEx(*font, word.c_str(), trueFontSize, fontSpacing).x);
    }

    std::vector<std::string> lineVector = wordVector2LineVector(wordVector);
    fieldContainer->clearChildren();
    textContainers.clear();

    float maxLineWidth = 0.0f;
    for(unsigned i = 0; i < lineVector.size(); ++i){
        float xPos;
        if(textAlignment.first == AlignmentX::LEFT){xPos = 0.0f;};
        if(textAlignment.first == AlignmentX::CENTRE_X){xPos = 0.5f;};
        if(textAlignment.first == AlignmentX::RIGHT){xPos = 1.0f;}; 
        auto tc = std::make_unique<TextContainer>(Vector2{xPos, i == 0 ? 0.0f : 1.0f}, 0.0f, lineVector[i], absFontSize);
        tc->setOriginAlignment({textAlignment.first, AlignmentY::TOP});

        if(textContainers.empty()){
            textContainers.push_back(static_cast<TextContainer*>(fieldContainer->addChild(std::move(tc))));
        }
        else{
            textContainers.push_back(static_cast<TextContainer*>(textContainers.back()->addChild(std::move(tc))));
        }
        float currentLineWidth = textContainers[i]->getSize().x;
        maxLineWidth = currentLineWidth > maxLineWidth ? currentLineWidth : maxLineWidth;
    }
    if(doAutoWidth){
        updateFieldContainerHeight();
        fieldContainer->setSize(maxLineWidth, fieldContainer->getSize().y);
        ContainerBox::setSize(maxLineWidth+padding*2.0f, ContainerBox::getSize().y);
    }
}

void TextField::updateFieldContainerHeight(){
    float fieldTrueHeight = textContainers.back()->getRect().y - fieldContainer->getRect().y + textContainers.back()->getRect().height;
    float fieldScaledHeight = fieldTrueHeight/ContainerBox::getScreenMagnitude();
    fieldContainer->setSize(fieldContainer->getSize().x, fieldScaledHeight);
    if(doAutoWidth){
        ContainerBox::setSize(ContainerBox::getSize().x, fieldContainer->getSize().y+padding*2.0f);
    }
}

TextField::TextField(Vector2 fieldPosition, float fieldWidth, float fieldHeight, float fontSize,
        std::string text, float padding, std::pair<AlignmentX, AlignmentY> textAlignment, bool doAutoWidth): 
        ContainerBox({fieldPosition.x, fieldPosition.y}, fieldWidth, fieldHeight){
    absFontSize = fontSize;
    this->textAlignment = textAlignment;
    this->padding = padding;
    this->doAutoWidth = doAutoWidth;

    ContainerBox::addResizeListener(this);

    {
        float yPos;
        if(textAlignment.second == AlignmentY::TOP){yPos = 0.0f;};
        if(textAlignment.second == AlignmentY::CENTRE_Y){yPos = 0.5f;};
        if(textAlignment.second == AlignmentY::BOTTOM){yPos = 1.0f;};

        auto pc = std::make_unique<ContainerBox>(Vector2{0.5f, yPos}, padding, padding);
        pc->setOriginAlignment({AlignmentX::CENTRE_X, textAlignment.second});
        paddingContainer = static_cast<ContainerBox*>(ContainerBox::addChild(std::move(pc)));

        if(textAlignment.second == AlignmentY::TOP){yPos = 1.0f;};
        auto fc = std::make_unique<ContainerBox>(Vector2{0.5f, yPos}, fieldWidth-2.0f*padding, padding);
        fc->setOriginAlignment({AlignmentX::CENTRE_X, textAlignment.second});
        fieldContainer = static_cast<ContainerBox*>(paddingContainer->addChild(std::move(fc)));
    }
    setText(text);
}

TextField::~TextField(){
    ContainerBox::removeResizeListener(this);
}

void TextField::setText(std::string newText){
    wordVector = string2WordVector(newText);
    updateTextConatiners();
}

void TextField::setTextAlignment(std::pair<AlignmentX, AlignmentY> newTextAlignment){
    textAlignment = newTextAlignment;
    
    // should make below better organised and combine with the constructor code
    float yPos;
    if(textAlignment.second == AlignmentY::TOP){yPos = 0.0f;};
    if(textAlignment.second == AlignmentY::CENTRE_Y){yPos = 0.5f;};
    if(textAlignment.second == AlignmentY::BOTTOM){yPos = 1.0f;};
    paddingContainer->setOffsetY(yPos);
    paddingContainer->setOriginAlignment(textAlignment.second);

    if(textAlignment.second == AlignmentY::TOP){yPos = 1.0f;};
    fieldContainer->setOffsetY(yPos);
    fieldContainer->setOriginAlignment(textAlignment.second);
}

void TextField::Draw(){
    for(TextContainer* textContainer: textContainers){
        textContainer->Draw();
    }
}

void TextField::drawDebug(Color colour){
    ContainerBox::drawDebug(ColorLerp(colour, MAGENTA, 0.3f));
    fieldContainer->drawDebug(ColorLerp(colour, PURPLE, 0.3f));
    paddingContainer->drawDebug(ColorLerp(colour, RED, 0.3f));
}

void TextField::setSize(float newDecimalWidth, float newDecimalHeight, bool isSilent){
    ContainerBox::setSize(newDecimalWidth, newDecimalHeight, isSilent);
    fieldContainer->setSize(newDecimalWidth-2.0f*padding, newDecimalHeight-2.0f*padding);
    updateTextConatiners();
    updateFieldContainerHeight();
}

void TextField::setPadding(float newPadding){
    padding = newPadding;
}

void TextField::onScreenResize(){
    ContainerBox::onScreenResize();
    updateTextConatiners();
}

void TextField::onAnyResizeComplete(ContainerBox *source){
    if (!textContainers.empty() and source == textContainers.back()){
        updateFieldContainerHeight();
    }
}
