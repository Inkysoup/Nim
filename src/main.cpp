#include <raylib.h>
#include <raymath.h>

/* std */
#include <filesystem>
#include <iostream>
#include <string>
#include <memory>
#include <iterator>
#include <cmath>
#include <random>

/* gui classes */
#include "textContainer.hpp"
#include "button.hpp"
#include "textButton.hpp"
#include "timer.hpp"
#include "textField.hpp"
#include "speechBubble.hpp"
#include "gameBoard.hpp"

/* globals */
#include "constants.hpp"
using namespace constants;

namespace{
std::filesystem::path setProjectRoot(const std::filesystem::path& executablePath) {
    auto parent = executablePath.parent_path();
    if (!parent.empty() and parent.filename() == "bin") {
        return parent.parent_path();
    }
    return parent.empty() ? std::filesystem::current_path() : parent;
}
}

int main(int argc, char** argv) {
    std::filesystem::path executablePath = std::filesystem::absolute(argv[0]);
    std::filesystem::current_path(setProjectRoot(executablePath));

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    //
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    //
    InitWindow(400, 500, "Nim");
    ContainerBox::setScreenRatio(400/500);
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    Shader backgroundShader = LoadShader(0, TextFormat("src/stripeShader.fs"));


    GameBoard gameBoard{{0.5f,0.55f}, 0.85f, 0.9f, 0.075f, 0.125f};
    gameBoard.setOriginAlignment({ContainerBox::CENTRE_X, ContainerBox::CENTRE_Y}); 

    ContainerBox topBar{{0.5f,0.0f}, 1.0f, 0.2f};
    topBar.setOriginAlignment(ContainerBox::CENTRE_X);
    ContainerBox* insetTopBar = [&topBar](){
        auto itp = std::make_unique<ContainerBox>(Vector2{0.5f, 0.5f}, 0.8f, 0.1f);
        itp->setOriginAlignment({ContainerBox::CENTRE_X, ContainerBox::CENTRE_Y});
        return static_cast<ContainerBox*>(topBar.addChild(std::move(itp)));
    }();

    TextButton* resetButton = [&insetTopBar](){
        auto rb = std::make_unique<TextButton>(Vector2{0.0f, 0.0f}, 0.1f, 0.1f, redColour, "↺");
        return static_cast<TextButton*>(insetTopBar->addChild(std::move(rb)));
    }();
    SpeechBubble* resetWarning = [&resetButton](){
        auto sb = std::make_unique<SpeechBubble>(Vector2{0.5f,-0.3f}, Vector2{0.5f, 1.5f}, "Reset?", 1.0f, false, 0.25f, 0.15f);
        sb->setOriginAlignment({ContainerBox::CENTRE_X, ContainerBox::TOP});
        sb->setTextAlignment({ContainerBox::CENTRE_X, ContainerBox::TOP});
        return static_cast<SpeechBubble*>(resetButton->addChild(std::move(sb)));
    }();
    TextButton* confirmResetButton = [&resetWarning](){
        auto crb = std::make_unique<TextButton>(Vector2{0.5f, 1.0f}, 0.15f, 0.1f, limeColour, "Yes");
        crb->setOriginAlignment({ContainerBox::CENTRE_X, ContainerBox::CENTRE_Y});
        return static_cast<TextButton*>(resetWarning->addControl(std::move(crb)));
    }();
    resetWarning->hide();


    TextButton* submitButton = [&insetTopBar](){
        auto sb = std::make_unique<TextButton>(Vector2{1.0f, 0.0f}, 0.65f, 0.1f, sageColour, "Submit");
        sb->setOriginAlignment(ContainerBox::RIGHT);
        return static_cast<TextButton*>(insetTopBar->addChild(std::move(sb)));
    }();

    submitButton->bindOnClick([&gameBoard](){
        gameBoard.submit();
    });

    resetButton->bindOnClick([resetWarning, confirmResetButton](){
        resetWarning->hide();
        resetWarning->show();
        confirmResetButton->setActive();
    });
    confirmResetButton->bindOnClick([resetWarning, &gameBoard](){
        resetWarning->hide();
        gameBoard.resetState();
    });
    confirmResetButton->bindOnUnactive([resetWarning](){
        resetWarning->hide();
    });


    std::random_device rd;
    Vector2 screenSize = Vector2(GetScreenWidth(), GetScreenHeight());
    float screenMagnitude = ContainerBox::getScreenMagnitude();
    ContainerBox::onScreenResizeManager();


    Vector2 mousePos;

    while (!WindowShouldClose()) // main loop
    {
        if(IsWindowResized()){
            screenSize = ContainerBox::getScreenSize();
            screenMagnitude = ContainerBox::getScreenMagnitude();
            ContainerBox::onScreenResizeManager();
        }

        mousePos = GetMousePosition();
        bool mouseUp = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);


        for(UIElement* element : UIElement::getAllElements()){
            if(element->isPressed(mousePos, mouseUp, mouseDown)){
                break;
            }
        }
        if(UIElement::getActive() != nullptr){
            if(mouseDown == true and UIElement::getActive()->getButtonState() != UIElement::HELD){
                UIElement::clearActive();
            }
        }

        int keyPressed = GetKeyPressed();
        while (keyPressed > 0){
            if(IsKeyDown(KEY_ENTER) and UIElement::getActive() != nullptr){
                UIElement::getActive()->setButtonState(UIElement::HELD);
            }
            if(keyPressed==KEY_F11){
                ToggleBorderlessWindowed();
                screenSize = Vector2(GetScreenWidth(), GetScreenHeight());
                screenMagnitude = ContainerBox::getScreenMagnitude();
                ContainerBox::onScreenResizeManager();
            }
            else if(keyPressed==KEY_TAB){
                if(IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
                    UIElement::progressActive(-1);
                }
                else{
                    UIElement::progressActive(1);
                }
            }
            else if(keyPressed == KEY_ESCAPE){
                UIElement::clearActive();
            }
            else if (UIElement::getActive() != nullptr and keyPressed > 0){
                UIElement::getActive()->handleKeyboardInput(keyPressed);
            }
            keyPressed = GetKeyPressed();
        }

        gameBoard.update();

        if(UIElement::getActive() != nullptr){
            if(UIElement::getActive()->getIsDisabled()){
                UIElement::clearActive();
            }
        }

        BeginDrawing();
            ClearBackground(WHITE);
            BeginShaderMode(backgroundShader);
                unsigned int timeUniformLocation = GetShaderLocation(backgroundShader, "time");
                float time = GetTime();
                SetShaderValue(backgroundShader, timeUniformLocation, &time, SHADER_UNIFORM_FLOAT);
                
                unsigned int colour1UniformLocation = GetShaderLocation(backgroundShader, "colour1");
                Vector3 colour1 = Vector3(tanColour.r/255.0f, tanColour.g/255.0f, tanColour.b/255.0f) ;
                SetShaderValue(backgroundShader, colour1UniformLocation, &colour1, SHADER_UNIFORM_VEC3);
                unsigned int colour2UniformLocation = GetShaderLocation(backgroundShader, "colour2");
                Vector3 colour2 = Vector3(mediumTanColour.r/255.0f, mediumTanColour.g/255.0f, mediumTanColour.b/255.0f);
                SetShaderValue(backgroundShader, colour2UniformLocation, &colour2, SHADER_UNIFORM_VEC3);

                unsigned int screenMagnitudeUniformLocation = GetShaderLocation(backgroundShader, "screenMagnitude");
                SetShaderValue(backgroundShader, screenMagnitudeUniformLocation, &screenMagnitude, SHADER_UNIFORM_FLOAT);
                
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RED);
            EndShaderMode();


            gameBoard.Draw();
            for(UIElement* uiElement: UIElement::getAllElements()) {
                if(uiElement->getIsAutoDraw()) uiElement->Draw();
            }
            resetWarning->Draw();
            UIElement::drawActive();
        EndDrawing();
    };
    
    UnloadShader(backgroundShader);
    CloseWindow();
}