#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include "Button.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "InputManager.h"
#include <SDL_ttf.h>
#include "GameConfig.h"
#include "AsteroidsGameplay.h"

class AsteroidsPauseScene : public Scene
{
private:
    Button* btnResume = nullptr;
    SDL_Texture* backgroundTex = nullptr;
    TTF_Font* font = nullptr;

public:
    AsteroidsPauseScene() = default;
    ~AsteroidsPauseScene() override = default;

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
            AsteroidsGameplay* g = dynamic_cast<AsteroidsGameplay*>(SM.GetScene("Asteroids"));
            if (g) g->ForceCleanup();
            SM.SetNextScene("MainMenu");
            return;
        }

        if (btnResume) btnResume->Update();

        if (btnResume && btnResume->IsClicked())
        {
            SM.SetNextScene("Asteroids");
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