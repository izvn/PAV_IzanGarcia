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
    TTF_Font* font;
    SDL_Texture* titleTexture;
    int titleW, titleH;
    SDL_Texture* labelBgTex;
    int labelBgW, labelBgH;
    SDL_Texture* labelEnTex;
    int labelEnW, labelEnH;
    SDL_Texture* labelVfxTex;
    int labelVfxW, labelVfxH;
    std::vector<ImageButton*> bgButtons;
    std::vector<ImageButton*> enButtons;
    std::vector<ImageButton*> vfxButtons;
    Button* btnBack;
public:
    SpriteSelector()
        : backgroundTexture(nullptr), font(nullptr),
        titleTexture(nullptr), titleW(0), titleH(0),
        labelBgTex(nullptr), labelBgW(0), labelBgH(0),
        labelEnTex(nullptr), labelEnW(0), labelEnH(0),
        labelVfxTex(nullptr), labelVfxW(0), labelVfxH(0),
        btnBack(nullptr) {}
    void OnEnter() override {
        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));
        RM.LoadFont("resources/fonts/fuente.otf", 28);
        font = RM.GetFont("resources/fonts/fuente.otf");
        titleTexture = CreateTextTexture("SELECT SPRITES", font, &titleW, &titleH);
        labelBgTex = CreateTextTexture("Background", font, &labelBgW, &labelBgH);
        labelEnTex = CreateTextTexture("Enemies", font, &labelEnW, &labelEnH);
        labelVfxTex = CreateTextTexture("VFX", font, &labelVfxW, &labelVfxH);
        bgButtons = {
            new ImageButton(GameConfig::GetBackgroundPath(0), 300, 150, 64, 64),
            new ImageButton(GameConfig::GetBackgroundPath(1), 380, 150, 64, 64),
            new ImageButton(GameConfig::GetBackgroundPath(2), 460, 150, 64, 64)
        };
        enButtons = {
            new ImageButton(GameConfig::GetEnemyPath(0), 300, 250, 64, 64),
            new ImageButton(GameConfig::GetEnemyPath(1), 380, 250, 64, 64),
            new ImageButton(GameConfig::GetEnemyPath(2), 460, 250, 64, 64)
        };
        vfxButtons = {
            new ImageButton(GameConfig::GetVFXPath(0), 300, 350, 64, 64),
            new ImageButton(GameConfig::GetVFXPath(1), 380, 350, 64, 64),
            new ImageButton(GameConfig::GetVFXPath(2), 460, 350, 64, 64)
        };
        btnBack = new Button("Back", 600, 600, 200, 40, font);
    }
    void OnExit() override {
        SDL_DestroyTexture(titleTexture);
        SDL_DestroyTexture(labelBgTex);
        SDL_DestroyTexture(labelEnTex);
        SDL_DestroyTexture(labelVfxTex);
        for (auto* b : bgButtons) delete b;
        bgButtons.clear();
        for (auto* b : enButtons) delete b;
        enButtons.clear();
        for (auto* b : vfxButtons) delete b;
        vfxButtons.clear();
        delete btnBack;
    }
    void Update() override {
        for (int i = 0; i < (int)bgButtons.size(); ++i) {
            bgButtons[i]->Update();
            if (bgButtons[i]->IsClicked()) {
                GameConfig::SetSelectedBackground(i);
                backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(i));
            }
        }
        for (int i = 0; i < (int)enButtons.size(); ++i) {
            enButtons[i]->Update();
            if (enButtons[i]->IsClicked()) {
                GameConfig::SetSelectedEnemies(i);
            }
        }
        for (int i = 0; i < (int)vfxButtons.size(); ++i) {
            vfxButtons[i]->Update();
            if (vfxButtons[i]->IsClicked()) {
                GameConfig::SetSelectedVFX(i);
            }
        }
        btnBack->Update();
        if (btnBack->IsClicked()) {
            SM.SetNextScene("MainMenu");
        }
    }
    void Render() override {
        SDL_RenderCopy(RM.GetRenderer(), backgroundTexture, nullptr, nullptr);
        RenderTexture(titleTexture, (1360 - titleW) / 2, 50, titleW, titleH);
        RenderTexture(labelBgTex, 50, 160, labelBgW, labelBgH);
        RenderTexture(labelEnTex, 50, 260, labelEnW, labelEnH);
        RenderTexture(labelVfxTex, 50, 360, labelVfxW, labelVfxH);
        for (auto* b : bgButtons) b->Render();
        for (auto* b : enButtons) b->Render();
        for (auto* b : vfxButtons) b->Render();
        btnBack->Render();
    }
private:
    SDL_Texture* CreateTextTexture(const std::string& text, TTF_Font* f, int* outW, int* outH) {
        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Surface* surf = TTF_RenderText_Blended(f, text.c_str(), white);
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
