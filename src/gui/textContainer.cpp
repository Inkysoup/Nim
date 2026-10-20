#include "textContainer.hpp"
#include "constants.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <numeric>

using namespace constants;

#define MAX_OUTLINES 3 // must be the same as in sdfText shader

std::shared_ptr<TextContainer::FontResources> TextContainer::fontResources = nullptr;

Font TextContainer::loadFontSDF(std::string asciiFont, std::vector<std::pair<std::string, std::vector<int>>> fontCodepoints){
    int baseSize = 24;
    int padding = 8;

    int asciiFontFileSize = 0;
    unsigned char *asciiFontFileData = LoadFileData(asciiFont.c_str(), &asciiFontFileSize);
    int *asciiFontCodepoints = NULL; // Default ASCII 32-126
    int asciiFontCharCount = 96;
    GlyphInfo *asciiFontglyphs = LoadFontData(asciiFontFileData, asciiFontFileSize, baseSize, asciiFontCodepoints, asciiFontCharCount, FONT_SDF, &asciiFontCharCount);

    std::vector<int> fontCharCounts;
    std::vector<GlyphInfo*> fontGlyphs;
    for(unsigned i = 0; i < fontCodepoints.size(); ++i){
        int fontFileSize = 0;
        unsigned char *fontFileData = LoadFileData(fontCodepoints[i].first.c_str(), &fontFileSize);
        fontCharCounts.push_back(fontCodepoints[i].second.size());
        fontGlyphs.push_back(LoadFontData(fontFileData, fontFileSize, baseSize, fontCodepoints[i].second.data(), fontCharCounts[i], FONT_SDF, &fontCharCounts[i]));
    }

    int totalChars = asciiFontCharCount + std::accumulate(fontCharCounts.begin(), fontCharCounts.end(), 0);
    GlyphInfo *allGlyphs = (GlyphInfo *)RL_MALLOC(totalChars * sizeof(GlyphInfo));

    GlyphInfo *glyphCursor = allGlyphs;
    std::memcpy(glyphCursor, asciiFontglyphs, asciiFontCharCount * sizeof(GlyphInfo));
    glyphCursor += asciiFontCharCount;
    for(unsigned i = 0; i < fontCodepoints.size(); ++i){
        std::memcpy(glyphCursor, fontGlyphs[i], fontCharCounts[i] * sizeof(GlyphInfo));
        glyphCursor += fontCharCounts[i];
    }

    Rectangle *recs = NULL;
    Image atlasImage = GenImageFontAtlas(allGlyphs, &recs, totalChars, baseSize, padding, 1);

    Font fontSDF = { 0 };
    fontSDF.baseSize = baseSize;
    fontSDF.glyphCount = totalChars;
    fontSDF.glyphs = allGlyphs;
    fontSDF.recs = recs;
    fontSDF.texture = LoadTextureFromImage(atlasImage);

    UnloadImage(atlasImage);
    UnloadFontData(asciiFontglyphs, asciiFontCharCount);
    for(unsigned i = 0; i < fontCodepoints.size(); ++i){
        UnloadFontData(fontGlyphs[i], fontCharCounts[i]);
    }

    SetTextureFilter(fontSDF.texture, TEXTURE_FILTER_BILINEAR);
    return fontSDF;
}

TextContainer::TextContainer(Vector2 containerPosition, float padding, std::string text, float fontSize, 
        std::pair<AlignmentX, AlignmentY> alignment, Color textColour, float textAlpha) :
        ContainerBox{containerPosition, 0.0f, 0.0f}{
    if(!TextContainer::fontResources){
        TextContainer::fontResources = std::make_shared<TextContainer::FontResources>();
        std::string fontRoot = "assets/fonts/";
        std::vector<std::pair<std::string, std::vector<int>>> fontCodepoints = {
            {fontRoot + "NotoSansMath-Regular.ttf", {0x21BA,}}, // ↺ = 0x21BA;
            {fontRoot + "NotoSansSymbols-Regular.ttf", {0x26ED,}}, // ⛭ = 0x26ED;
        };
        TextContainer::fontResources->font = loadFontSDF(fontRoot + "NotoSans-Regular.ttf", fontCodepoints);
        TextContainer::fontResources->shader = LoadShader(0, TextFormat("src/gui/sdfText.fs"));
    }
    this->padding = padding;
    this->text = text;
    this->setAbsFontSize(fontSize);
    ContainerBox::setOriginAlignment(alignment);
    colour = textColour;
    alpha = textAlpha;
    this->onScreenResize(); //replace with better later
}

void TextContainer::resizeContainerBox(){
    Vector2 textSize = MeasureTextEx(TextContainer::fontResources->font, text.c_str(), fontSize, fontSpacing);
    float screenMag = ContainerBox::getScreenMagnitude();
    ContainerBox::setSize((textSize.x + padding*2.0f) / screenMag, (textSize.y + padding*2.0f) / screenMag);
}

TextContainer::~TextContainer(){
}

void TextContainer::Draw(){
    BeginShaderMode(TextContainer::fontResources->shader);
    float runningTotalExpansionFract = 0.0f;
        for(unsigned i = 0; i <= MAX_OUTLINES; ++i){
            float expansionFract = 0.0f;
            Vector4 rgba = {0.0f, 0.0f, 0.0f, 0.0f};
            unsigned int expansionFractUniformLocation = GetShaderLocation(TextContainer::fontResources->shader, TextFormat("outlines[%d].expansionFract", MAX_OUTLINES - i));
            unsigned int outlineColourUniformLocation = GetShaderLocation(TextContainer::fontResources->shader, TextFormat("outlines[%d].colour", MAX_OUTLINES - i));
            int outlineIndex = static_cast<int>(i) - MAX_OUTLINES + static_cast<int>(outlines.size()) - 1;
            if(outlineIndex == -1){
                rgba = {
                    colour.r/255.0f,
                    colour.g/255.0f,
                    colour.b/255.0f,
                    colour.a/255.0f
                }; 
            }
            else if(outlineIndex >= 0){
                runningTotalExpansionFract += outlines[outlineIndex].first;
                expansionFract = runningTotalExpansionFract;
                Color outlineColour = outlines[outlineIndex].second;
                rgba = {
                    outlineColour.r/255.0f,
                    outlineColour.g/255.0f,
                    outlineColour.b/255.0f,
                    outlineColour.a/255.0f
                };
            }
            SetShaderValue(TextContainer::fontResources->shader, expansionFractUniformLocation, &expansionFract, SHADER_UNIFORM_FLOAT);
            SetShaderValue(TextContainer::fontResources->shader, outlineColourUniformLocation, &rgba, SHADER_UNIFORM_VEC4);
        }
        Rectangle rect = ContainerBox::getRect();
        DrawTextEx(TextContainer::fontResources->font, text.c_str(), {rect.x + padding, rect.y + padding}, 
            fontSize, fontSpacing, Color{255, 255, 255, static_cast<unsigned char>(alpha * 255.0f)});
    EndShaderMode();
}

void TextContainer::setText(std::string newText){
    text.swap(newText);
    this->resizeContainerBox();
}

std::string TextContainer::getText(){
    return text;
}

Font* TextContainer::getSDFFont(){
    return &fontResources->font;
}

void TextContainer::setAbsFontSize(float newAbsFontSize){
    absFontSize = newAbsFontSize;
    onScreenResize();
}

float TextContainer::getAbsFontSize(){
    return absFontSize;
}

float TextContainer::getFontSize(){
    return fontSize;
}

float TextContainer::getFontSpacing(){
    return fontSpacing;
}

float TextContainer::calculateTrueFontSize(float absFontSize){
    return std::round((absFontSize/400.0f * ContainerBox::getScreenMagnitude()) / 6.0f) * 6.0f;
}

float TextContainer::calculateFontSpacing(float trueFontSize){
    return trueFontSize/24.0f;
}

Vector2 TextContainer::getTextSize(){
    return MeasureTextEx(TextContainer::fontResources->font, text.c_str(), fontSize, fontSpacing);
}

void TextContainer::setPadding(float newPadding){
    padding = newPadding;
}

void TextContainer::setColour(Color newColour){
    colour = newColour;
}

Color TextContainer::getColour(){
    return colour;
}

void TextContainer::setAlpha(float newAlpha){
    alpha = newAlpha;
}

void TextContainer::appendOutline(float outlineFract, Color colour){
    if(outlines.size() < MAX_OUTLINES){
        outlines.push_back({outlineFract, colour});
    }
}

void TextContainer::removeOutline(unsigned index){
    if(outlines.size() > index){
        outlines.erase(outlines.begin() + index);
    }
}

void TextContainer::onScreenResize(){
    fontSize = calculateTrueFontSize(absFontSize);
    fontSpacing = calculateFontSpacing(fontSize);
    resizeContainerBox();
    ContainerBox::onScreenResize();
}
