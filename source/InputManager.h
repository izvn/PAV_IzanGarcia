#pragma once
#include <SDL.h>
#include <unordered_map>
#include <iostream>
#include <string>

enum KeyState { EMPTY, DOWN, UP, HOLD, RELEASED };

#define Input InputManager::Instance()

class InputManager {
private:
    int mouseX, mouseY;
    bool leftClick;
    std::unordered_map<Sint32, KeyState> keyReference;
    std::string inputText;
    bool enterPressed;

    InputManager() {
        SDL_GetMouseState(&mouseX, &mouseY);
        leftClick = false;
        enterPressed = false;
        inputText = "";
    }
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

public:
    static InputManager& Instance() {
        static InputManager instance;
        return instance;
    }

    bool Listen() {
        enterPressed = false;
        for (auto& k : keyReference) {
            if (k.second == DOWN) k.second = HOLD;
            else if (k.second == UP) k.second = RELEASED;
        }

        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return true;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) leftClick = true;
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) leftClick = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                keyReference[event.key.keysym.sym] = DOWN;
                if (event.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty()) {
                    inputText.pop_back();
                }
                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                    enterPressed = true;
                }
            }
            else if (event.type == SDL_KEYUP) {
                keyReference[event.key.keysym.sym] = UP;
            }
            else if (event.type == SDL_TEXTINPUT) {
                if (inputText.length() < 12) {
                    inputText += event.text.text;
                }
            }
        }
        return false;
    }

    inline int GetMouseX() const { return mouseX; }
    inline int GetMouseY() const { return mouseY; }
    inline bool GetLeftClick() const { return leftClick; }
    inline bool GetEvent(Sint32 input, KeyState inputValue) {
        if (keyReference.find(input) == keyReference.end()) return false;
        return keyReference[input] == inputValue;
    }
    inline std::string GetInputText() const { return inputText; }
    inline void ClearInputText() { inputText = ""; }
    inline bool GetEnterPressed() const { return enterPressed; }
    inline void StartTextInput() { SDL_StartTextInput(); }
    inline void StopTextInput() { SDL_StopTextInput(); }
};