#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "GameConfig.h"
#include "RankingScene.h"
#include "TimeManager.h"
#include <SDL_ttf.h>
#include <string>

class GameOverScene : public Scene
{
private:
    SDL_Texture* backgroundTex = nullptr;
    TTF_Font* fontTitle = nullptr;
    TTF_Font* fontNormal = nullptr;

    SDL_Texture* titleTex = nullptr;
    SDL_Texture* instructionTex = nullptr;
    SDL_Texture* scoreTex = nullptr;
    SDL_Texture* usernameTex = nullptr;

    int titleW, titleH;
    int instW, instH;
    int scoreW, scoreH;
    int usernameW, usernameH;


    float cursorTimer = 0.0f;
    bool showCursor = true;

public:
    GameOverScene() = default;
    ~GameOverScene() override = default;

    void OnEnter() override
    {
        backgroundTex = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));

        RM.LoadFont("resources/fonts/fuente.otf", 48);
        fontTitle = RM.GetFont("resources/fonts/fuente.otf");
        RM.LoadFont("resources/fonts/fuente.otf", 28);
        fontNormal = RM.GetFont("resources/fonts/fuente.otf");

        titleTex = CreateTextTexture("GAME OVER", fontTitle, &titleW, &titleH);
        instructionTex = CreateTextTexture("Press ENTER to save and continue", fontNormal, &instW, &instH);

        char buffer[64];
        sprintf_s(buffer, "Score: %d", GameConfig::pendingScore);
        scoreTex = CreateTextTexture(buffer, fontNormal, &scoreW, &scoreH);

        Input.ClearInputText();
        Input.StartTextInput();

        cursorTimer = 0.0f;
        showCursor = true;
    }

    void OnExit() override
    {
        Input.StopTextInput();

        if (titleTex) SDL_DestroyTexture(titleTex);
        if (instructionTex) SDL_DestroyTexture(instructionTex);
        if (scoreTex) SDL_DestroyTexture(scoreTex);
    }

    void Update() override
    {
        cursorTimer += TIME.GetDeltaTime();
        if (cursorTimer >= 0.5f) {
            showCursor = !showCursor;
            cursorTimer = 0.0f;
        }

        usernameTex = CreateTextTexture(Input.GetInputText(), fontNormal, &usernameW, &usernameH);


        if (Input.GetEvent(SDLK_RETURN, DOWN) || Input.GetEvent(SDLK_KP_ENTER, DOWN))
        {
            RankingScene::InsertScoreForMode(GameConfig::pendingMode, Input.GetInputText(), GameConfig::pendingScore);
            SM.SetNextScene("MainMenu");
        }
    }

    void Render() override
    {
        if (backgroundTex)
        {
            SDL_Rect destRect = { 0, 0, RM.WINDOW_WIDTH, RM.WINDOW_HEIGHT };
            SDL_RenderCopy(RM.GetRenderer(), backgroundTex, nullptr, &destRect);
        }

        RenderTexture(titleTex, (RM.WINDOW_WIDTH - titleW) / 2, 100, titleW, titleH);
        RenderTexture(scoreTex, (RM.WINDOW_WIDTH - scoreW) / 2, 200, scoreW, scoreH);

     

        SDL_Rect boxRect = { (RM.WINDOW_WIDTH - 400) / 2, 300, 400, 60 };
        SDL_SetRenderDrawColor(RM.GetRenderer(), 80, 80, 80, 255);
        SDL_RenderFillRect(RM.GetRenderer(), &boxRect);
        SDL_SetRenderDrawColor(RM.GetRenderer(), 255, 255, 255, 255);
        SDL_RenderDrawRect(RM.GetRenderer(), &boxRect);

       
        RenderTexture(usernameTex, boxRect.x + 20, boxRect.y + (boxRect.h - usernameH) / 2, usernameW, usernameH);

        RenderTexture(instructionTex, (RM.WINDOW_WIDTH - instW) / 2, 450, instW, instH);
    }

private:
    SDL_Texture* CreateTextTexture(const std::string& text, TTF_Font* font, int* outW, int* outH) {
        if (!font || text.empty()) return nullptr;
        SDL_Color color = { 255, 255, 255, 255 };
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