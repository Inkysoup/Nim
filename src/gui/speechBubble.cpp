#include "speechBubble.hpp"
#include <raymath.h>
#include <rlgl.h>

const float startAnimDuration = 0.1f;

SpeechBubble::SpeechBubble(Vector2 stemTipOffset, Vector2 bubblePosition, std::string text, float stemWidthPercentage, 
        bool doAutoWidth, float BubbleWidth, float BubbleHeight):
        TextField(bubblePosition, BubbleWidth, BubbleHeight, 24.0f, text, 0.02f, {AlignmentX::CENTRE_X, AlignmentY::CENTRE_Y}, doAutoWidth),
        animTimer(startAnimDuration){
    animTimer.resume();
    this->stemWidthPercentage = stemWidthPercentage;
    {
        auto st = std::make_unique<ContainerBox>(stemTipOffset, 0.0f, 0.0f);
        stemTip = static_cast<ContainerBox*>(TextField::addChild(std::move(st)));
    }
}

SpeechBubble::~SpeechBubble(){
}

void SpeechBubble::hide(){
    animTimer.pause();
    animTimer.setSecondsRemaining(startAnimDuration);
    for(auto &control: controls){
        control->disable();
    }
}

void SpeechBubble::show(){
    animTimer.resume();
    for(auto &control: controls){
        control->enable();
    }
}

void SpeechBubble::Draw(){
    Rectangle rect = TextField::getRect();
    Vector2 rectCentre = {rect.x+rect.width*0.5f, rect.y+rect.height*0.5f};
    Vector2 stemTipTruePoition = {stemTip->getRect().x, stemTip->getRect().y};
    float bubbleMagnitde = std::min(TextField::getSize().x, TextField::getSize().y);
    float stemTrueRadius = stemWidthPercentage*0.5f*bubbleMagnitde*getScreenMagnitude();

    Vector2 dirTip2Centre = Vector2Normalize(Vector2Subtract(rectCentre, stemTipTruePoition));
    Vector2 Centre2Pt = Vector2Scale({-dirTip2Centre.y, dirTip2Centre.x}, stemTrueRadius);
    Vector2 v1 = Vector2Add(rectCentre,Centre2Pt);
    Vector2 v3 = Vector2Subtract(rectCentre,Centre2Pt);

    float startAnimProgress = (startAnimDuration-animTimer.getSecondsRemaining())/startAnimDuration;
    float t1 = startAnimProgress;
    t1 = t1 * (2.0 - t1);
    // float t2 = startAnimProgress;
    // t2 = t2*t2;
    rlPushMatrix();
    rlTranslatef(stemTipTruePoition.x, stemTipTruePoition.y, 0.0f);
    rlScalef(t1, t1, 1.0f);
    rlTranslatef(-stemTipTruePoition.x, -stemTipTruePoition.y, 0.0f);
        DrawLineEx(v1, stemTipTruePoition, ContainerBox::getOutlineWidth()*2.0f, BLACK);
        DrawLineEx(stemTipTruePoition, v3, ContainerBox::getOutlineWidth()*2.0f, BLACK);
        DrawCircle(stemTipTruePoition.x, stemTipTruePoition.y, ContainerBox::getOutlineWidth(), BLACK);
        DrawRectangleRounded(rect, 0.2f, 5, WHITE);
        DrawRectangleRoundedLinesEx(rect, 0.2f, 5, ContainerBox::getOutlineWidth(), BLACK);
        DrawTriangle(v3, stemTipTruePoition, v1, WHITE);
        TextField::Draw();
        for(auto &control: controls){
            control->Draw();
        }
    rlPopMatrix();
}

UIElement* SpeechBubble::addControl(std::unique_ptr<UIElement> control){
    controls.push_back(static_cast<UIElement*>(this->addChild(std::move(control))));
    controls.back()->setAutoDraw(false);
    return controls.back();
}

void SpeechBubble::setStemTipPosition(Vector2 newOffset){
    stemTip->setOffset(newOffset);
}
