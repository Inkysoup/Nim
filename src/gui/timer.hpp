#pragma once

#include <chrono>
// #include <functional>

class Timer{
private:
    bool isCounting;
    float secondsRemaining;
    std::chrono::steady_clock::time_point previousTime;
    // std::function<void(void)> onEndFunc;
public:
    Timer(float durationSeconds);
    ~Timer();
    void pause();
    void resume();
    bool getIsCounting();
    float getSecondsRemaining();
    void setSecondsRemaining(float newSecondsRemaing);
    void addSecondsRemaing(float addedSeconds);
    void update();
    // void bindOnEnd(std::function<void(void)> func);
};