#pragma once
#include <SDL.h>
#include <cmath>

#define TIME TimeManager::Instance()

class TimeManager {
private:
    float deltaTime = 0.0f;
    float elapsedTime = 0.0f;
    float previousElapsedTime = 0.0f;

    TimeManager() {
        previousElapsedTime = (float)SDL_GetTicks() / 1000.0f;
    }
    ~TimeManager() = default;
    TimeManager(const TimeManager&) = delete;
    TimeManager& operator=(const TimeManager&) = delete;

public:
    static TimeManager& Instance() {
        static TimeManager tm;
        return tm;
    }

    inline float GetDeltaTime() const { return deltaTime; }
    inline float GetElapsedTime() const { return elapsedTime; }

    void Update() {
        elapsedTime = (float)SDL_GetTicks() / 1000.0f;
        deltaTime = elapsedTime - previousElapsedTime;
        previousElapsedTime = elapsedTime;
    }
};
