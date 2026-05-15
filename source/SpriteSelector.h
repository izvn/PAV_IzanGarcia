#pragma once
#include "Scene.h"
#include <SDL.h>
#include <vector>
#include "RenderManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Button.h"
#include "ImageButton.h"
#include "GameConfig.h"

class SpriteSelector : public Scene {
private:
    SDL_Texture* backgroundTexture;
    TTF_Font* fontTitle;
    SDL_Texture* titleTexture;
    int titleW, titleH;

    std::vector<ImageButton*> bgButtons;
    Button* btnBack;

public:
    SpriteSelector()
        : backgroundTexture(nullptr), fontTitle(nullptr),
        titleTexture(nullptr), titleW(0), titleH(0),
        btnBack(nullptr) {
    }

    void OnEnter() override {
        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));

        RM.LoadFont("resources/fonts/fuente.otf", 48);
        fontTitle = RM.GetFont("resources/fonts/fuente.otf");
        titleTexture = CreateTextTexture("SELECT BACKGROUND", fontTitle, &titleW, &titleH);

        int btnSize = 250;
        int spacing = (1360 - (btnSize * 3)) / 4;
        int startY = 220;

        bgButtons = {
            new ImageButton(GameConfig::GetBackgroundPath(0), spacing, startY, btnSize, btnSize),
            new ImageButton(GameConfig::GetBackgroundPath(1), spacing * 2 + btnSize, startY, btnSize, btnSize),
            new ImageButton(GameConfig::GetBackgroundPath(2), spacing * 3 + btnSize * 2, startY, btnSize, btnSize)
        };

        RM.LoadFont("resources/fonts/fuente.otf", 28);
        TTF_Font* fontBack = RM.GetFont("resources/fonts/fuente.otf");
        btnBack = new Button("Back to Menu", (1360 - 250) / 2, 680, 250, 45, fontBack);
    }

    void OnExit() override {
        if (titleTexture) SDL_DestroyTexture(titleTexture);
        for (auto* b : bgButtons) delete b;
        bgButtons.clear();
        if (btnBack) delete btnBack;
    }

    void Update() override {
        for (int i = 0; i < (int)bgButtons.size(); ++i) {
            bgButtons[i]->Update();
            if (bgButtons[i]->IsClicked()) {
                GameConfig::SetSelectedBackground(i);
                backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(i));
            }
        }

        btnBack->Update();
        if (btnBack->IsClicked()) {
            SM.SetNextScene("MainMenu");
        }
    }

    void Render() override {
        if (backgroundTexture) {
            SDL_RenderCopy(RM.GetRenderer(), backgroundTexture, nullptr, nullptr);
        }

        RenderTexture(titleTexture, (1360 - titleW) / 2, 80, titleW, titleH);

        for (auto* b : bgButtons) b->Render();
        btnBack->Render();
    }

private:
    SDL_Texture* CreateTextTexture(const std::string& text, TTF_Font* f, int* outW, int* outH) {
        SDL_Color color = { 0, 0, 0, 255 };
        SDL_Surface* surf = TTF_RenderText_Blended(f, text.c_str(), color);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(RM.GetRenderer(), surf);
        *outW = surf->w;
        *outH = surf->h;
        SDL_FreeSurface(surf);
        return tex;
    }

    void RenderTexture(SDL_Texture* tex, int x, int y, int w, int h) {
        SDL_Rect dest = { x, y, w, h };
        SDL_RenderCopy(RM.GetRenderer(), tex, nullptr, &dest);
    }
};