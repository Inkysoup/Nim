#include "timer.hpp"

Timer::Timer(float durationSeconds){
    isCounting = false;
    secondsRemaining = durationSeconds;
    onEndFunc = [](){};
}

Timer::~Timer(){
}

void Timer::pause(){
    isCounting = false;
}

void Timer::resume(){
    isCounting = true;
    previousTime = std::chrono::steady_clock::now();
}

bool Timer::getIsCounting(){
    return isCounting;
}

float Timer::getSecondsRemaining(){
    update();
    return secondsRemaining;
}

void Timer::setSecondsRemaining(float newSecondsRemaing){
    secondsRemaining = newSecondsRemaing;
}

void Timer::addSecondsRemaing(float addedSeconds){
    secondsRemaining += addedSeconds;
}

void Timer::update(){
    if(isCounting and secondsRemaining > 0.0f){
        auto now = std::chrono::steady_clock::now();
        secondsRemaining -= std::chrono::duration<float>(now - previousTime).count();
        secondsRemaining = std::max(secondsRemaining, 0.0f);
        previousTime = now;
        if(secondsRemaining == 0.0f){
            isCounting = false;
            onEndFunc();
        }
    }
}

void Timer::bindOnEnd(std::function<void(void)> func){
    onEndFunc = func;
}
