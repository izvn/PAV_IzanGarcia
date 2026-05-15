#pragma once
#include "Scene.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "GameConfig.h"
#include "RankingScene.h"
#include "AudioManager.h"
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class GameOverScene : public Scene {
private:
    SDL_Texture* backgroundTexture;
    SDL_Texture* titleTexture;
    SDL_Texture* scoreTexture;
    SDL_Texture* promptTexture;

    int titleW, titleH;
    int scoreW, scoreH;
    int promptW, promptH;

    float blinkTimer;
    bool showPrompt;

public:
    GameOverScene() : backgroundTexture(nullptr), titleTexture(nullptr), scoreTexture(nullptr), promptTexture(nullptr),
        titleW(0), titleH(0), scoreW(0), scoreH(0), promptW(0), promptH(0), blinkTimer(0.0f), showPrompt(true) {
    }

    void OnEnter() override {
        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));

        RM.LoadFont("resources/fonts/fuente.otf", 64);
        TTF_Font* fontTitle = RM.GetFont("resources/fonts/fuente.otf");

        RM.LoadFont("resources/fonts/fuente.otf", 36);
        TTF_Font* fontScore = RM.GetFont("resources/fonts/fuente.otf");

        RM.LoadFont("resources/fonts/fuente.otf", 24);
        TTF_Font* fontPrompt = RM.GetFont("resources/fonts/fuente.otf");

        SDL_Color colorRed = { 255, 50, 50, 255 };
        SDL_Color colorWhite = { 255, 255, 255, 255 };
        SDL_Color colorGrey = { 200, 200, 200, 255 };

        titleTexture = CreateTextTexture("GAME OVER", fontTitle, &titleW, &titleH, colorRed);

        std::string scoreText = "FINAL SCORE: " + std::to_string(GameConfig::pendingScore);
        scoreTexture = CreateTextTexture(scoreText, fontScore, &scoreW, &scoreH, colorWhite);

        promptTexture = CreateTextTexture("PRESS ENTER TO CONTINUE", fontPrompt, &promptW, &promptH, colorGrey);

        blinkTimer = 0.0f;
        showPrompt = true;
    }

    void OnExit() override {
        if (titleTexture) SDL_DestroyTexture(titleTexture);
        if (scoreTexture) SDL_DestroyTexture(scoreTexture);
        if (promptTexture) SDL_DestroyTexture(promptTexture);

        titleTexture = nullptr;
        scoreTexture = nullptr;
        promptTexture = nullptr;
    }

    void Update() override {
        blinkTimer += TIME.GetDeltaTime();
        if (blinkTimer >= 0.5f) {
            showPrompt = !showPrompt;
            blinkTimer = 0.0f;
        }

        if (Input.GetEvent(SDLK_RETURN, DOWN) || Input.GetEvent(SDLK_KP_ENTER, DOWN)) {
            RankingScene::InsertScoreForMode(GameConfig::pendingMode, "Player", GameConfig::pendingScore);
            AM.PlayClip("button_click", 0);
            SM.SetNextScene("RankingScene");
        }
    }

    void Render() override {
        if (backgroundTexture) {
            SDL_RenderCopy(RM.GetRenderer(), backgroundTexture, nullptr, nullptr);
        }

        RenderTexture(titleTexture, (RM.WINDOW_WIDTH - titleW) / 2, 200, titleW, titleH);
        RenderTexture(scoreTexture, (RM.WINDOW_WIDTH - scoreW) / 2, 350, scoreW, scoreH);

        if (showPrompt) {
            RenderTexture(promptTexture, (RM.WINDOW_WIDTH - promptW) / 2, 500, promptW, promptH);
        }
    }

private:
    SDL_Texture* CreateTextTexture(const std::string& text, TTF_Font* font, int* outW, int* outH, SDL_Color color) {
        if (!font) return nullptr;
        SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surf) return nullptr;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(RM.GetRenderer(), surf);
        if (outW) *outW = surf->w;
        if (outH) *outH = surf->h;
        SDL_FreeSurface(surf);
        return tex;
    }

    void RenderTexture(SDL_Texture* tex, int x, int y, int w, int h) {
        if (!tex) return;
        SDL_Rect dst = { x, y, w, h };
        SDL_RenderCopy(RM.GetRenderer(), tex, nullptr, &dst);
    }
};