#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include "Button.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "InputManager.h"
#include <SDL_ttf.h>
#include "GameConfig.h"
#include "FlappyGameplay.h"

class FlappyPauseScene : public Scene
{
private:
    Button* btnResume = nullptr;
    SDL_Texture* backgroundTex = nullptr;
    TTF_Font* font = nullptr;

public:
    FlappyPauseScene() = default;
    ~FlappyPauseScene() override = default;

    void OnEnter() override
    {
        backgroundTex = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));

        RM.LoadFont("resources/fonts/fuente.otf", 28);
        font = RM.GetFont("resources/fonts/fuente.otf");

        int btnW = 200;
        int btnH = 40;
        int posX = (RM.WINDOW_WIDTH - btnW) / 2;
        int posY = (RM.WINDOW_HEIGHT - btnH) / 2;
        btnResume = new Button("Resume", posX, posY, btnW, btnH, font);
    }

    void OnExit() override
    {
        if (btnResume)
        {
            delete btnResume;
            btnResume = nullptr;
        }
    }

    void Update() override
    {
        if (Input.GetEvent(SDLK_ESCAPE, DOWN))
        {
            FlappyGameplay* g = dynamic_cast<FlappyGameplay*>(SM.GetScene("Flappy"));
            if (g) g->ForceCleanup();
            SM.SetNextScene("MainMenu");
            return;
        }

        if (btnResume) btnResume->Update();

        if (btnResume && btnResume->IsClicked())
        {
            SM.SetNextScene("Flappy");
            return;
        }
    }

    void Render() override
    {
        if (backgroundTex)
        {
            SDL_Rect destRect = { 0, 0, RM.WINDOW_WIDTH, RM.WINDOW_HEIGHT };
            SDL_RenderCopy(RM.GetRenderer(), backgroundTex, nullptr, &destRect);
        }

        if (btnResume)
        {
            btnResume->Render();
        }
    }
};