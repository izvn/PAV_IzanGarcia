#pragma once
#include "Scene.h"
#include "TimeManager.h"
#include "RenderManager.h"
#include <SDL_ttf.h>

class SplashScreen : public Scene {
private:
    float timer;
    TTF_Font* font;
    SDL_Color textColor;

public:
    SplashScreen() : timer(0.0f), font(nullptr), textColor({ 255, 255, 255, 255 }) {}

    void OnEnter() override {
        timer = 0.0f;
        font = TTF_OpenFont("resources/fonts/fuente.otf", 32);
        if (!font) {
            SDL_Log("Failed to load font: %s", TTF_GetError());
        }
    }

    void Update() override {
        timer += TIME.GetDeltaTime();
        if (timer >= 3.0f) {
            SM.SetNextScene("MainMenu");
        }
    }

    void Render() override {
        SDL_Texture* texture = RM.GetTexture("resources/SplashScreen.png");
        if (texture) {
            SDL_Rect destRect = { 0, 0, 1360, 768 };
            SDL_RenderCopy(RM.GetRenderer(), texture, nullptr, &destRect);
        }
    }

    void RenderText(const std::string& text, int x, int y) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(RM.GetRenderer(), textSurface);
            SDL_Rect textRect = { x - textSurface->w / 2, y, textSurface->w, textSurface->h };
            SDL_RenderCopy(RM.GetRenderer(), textTexture, nullptr, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
    }

    void OnExit() override {
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }
    }
};