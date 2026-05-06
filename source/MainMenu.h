#pragma once
#include "Scene.h"
#include "Button.h"
#include "SceneManager.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "GameConfig.h"
#include <vector>

class MainMenu : public Scene {
private:
    std::vector<Button*> buttons;
    SDL_Texture* backgroundTex;
    TTF_Font* font;
    TTF_Font* titleFont;

    float audioCooldownTimer;
    const float audioCooldownDuration = 0.5f;
    bool canToggleAudio;

    bool changeScenePending;
    std::string pendingScene;

    float sceneChangeDelay;
    const float delayDuration = 0.3f;

public:
    MainMenu()
        : backgroundTex(nullptr), font(nullptr), titleFont(nullptr),
        audioCooldownTimer(0.0f), canToggleAudio(true),
        changeScenePending(false), sceneChangeDelay(0.0f) {
    }

    void OnEnter() override {
        changeScenePending = false;
        sceneChangeDelay = 0.0f;

        RM.LoadFont("resources/fonts/fuente.otf", 28);
        font = RM.GetFont("resources/fonts/fuente.otf");
        RM.LoadFont("resources/fonts/fuente.otf", 48);
        titleFont = RM.GetFont("resources/fonts/fuente.otf");

        backgroundTex = RM.GetTexture("resources/mainmenu.png");

        AM.PlaySong("menu_music");

        buttons.push_back(new Button("Space Invaders", 200, 30, 200, 40, font));
        buttons.push_back(new Button("Tanks", 200, 80, 200, 40, font));
        buttons.push_back(new Button("Splat!", 200, 130, 200, 40, font));
        buttons.push_back(new Button("Asteroids", 200, 180, 200, 40, font));
        buttons.push_back(new Button("Breakout", 200, 230, 200, 40, font));
        buttons.push_back(new Button("Frogger", 200, 280, 200, 40, font));
        buttons.push_back(new Button("Space Arena", 200, 330, 200, 40, font));
        buttons.push_back(new Button("Scroller", 200, 380, 200, 40, font));
        buttons.push_back(new Button("Select Sprites", 200, 430, 200, 40, font));
        buttons.push_back(new Button("Check Ranking", 200, 480, 200, 40, font));
        std::string audioLabel = AM.GetMuted() ? "Audio Disabled" : "Audio Enabled";
        buttons.push_back(new Button(audioLabel, 200, 530, 200, 40, font));
        buttons.push_back(new Button("Exit", 200, 580, 200, 40, font));
    }

    void OnExit() override {
        AM.StopMusic();
        for (auto* b : buttons) {
            delete b;
        }
        buttons.clear();
    }

    void Update() override {
        if (!canToggleAudio) {
            audioCooldownTimer -= TIME.GetDeltaTime();
            if (audioCooldownTimer <= 0.0f) {
                canToggleAudio = true;
                audioCooldownTimer = 0.0f;
            }
        }

        if (changeScenePending) {
            sceneChangeDelay -= TIME.GetDeltaTime();
            if (sceneChangeDelay <= 0.0f) {
                SM.SetNextScene(pendingScene);
                return;
            }
        }

        for (auto* b : buttons) {
            b->Update();
            if (b->IsClicked() && !changeScenePending) {
                HandleButtonClick(b->GetLabel(), b);
            }
        }
    }

    void Render() override {
        SDL_Rect dest = { 0, 0, RM.WINDOW_WIDTH, RM.WINDOW_HEIGHT };
        SDL_RenderCopy(RM.GetRenderer(), backgroundTex, nullptr, &dest);
        for (auto* b : buttons) {
            b->Render();
        }
    }

    void HandleButtonClick(const std::string& label, Button* button) {
        if (label == "Space Invaders") PrepareSceneChange("SpaceInvaders");
        else if (label == "Tanks") PrepareSceneChange("Tanks");
        else if (label == "Splat!") PrepareSceneChange("Splat");
        else if (label == "Asteroids") PrepareSceneChange("Asteroids");
        else if (label == "Breakout") PrepareSceneChange("Breakout");
        else if (label == "Frogger") PrepareSceneChange("Frogger");
        else if (label == "Space Arena") PrepareSceneChange("Arena");
        else if (label == "Scroller") PrepareSceneChange("Scroller");
        else if (label == "Select Sprites") PrepareSceneChange("SpriteSelector");
        else if (label == "Check Ranking") PrepareSceneChange("RankingScene");
        else if (label == "Audio Enabled" || label == "Audio Disabled") {
            if (AM.GetMuted()) {
                AM.Unmute();
                button->SetLabel("Audio Enabled");
            }
            else {
                AM.Mute();
                button->SetLabel("Audio Disabled");
            }
        }
        else if (label == "Exit") {
            SDL_Event quitEvent;
            quitEvent.type = SDL_QUIT;
            SDL_PushEvent(&quitEvent);
        }
    }

    void PrepareSceneChange(const std::string& sceneName) {
        pendingScene = sceneName;
        changeScenePending = true;
        sceneChangeDelay = delayDuration;
        AM.PlayClip("button_click", 0);
    }
};