#pragma once
#include "Scene.h"
#include <SDL.h>
#include "RenderManager.h"
#include "GameConfig.h"
#include "Button.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstring>

struct ScoreRecord {
    char name[16];
    int score;
};

class RankingScene : public Scene {
private:
    ScoreRecord spaceScores[10];
    ScoreRecord tanksScores[10];
    ScoreRecord splatScores[10];
    ScoreRecord asteroidsScores[10];

    Button* btnBack;

    TTF_Font* fontTitle;
    SDL_Texture* titleTexture;
    int titleW, titleH;

    SDL_Texture* spaceTitleTex;
    SDL_Texture* tanksTitleTex;
    SDL_Texture* splatTitleTex;
    SDL_Texture* asteroidsTitleTex;
    int spaceTitleW, spaceTitleH;
    int tanksTitleW, tanksTitleH;
    int splatTitleW, splatTitleH;
    int asteroidsTitleW, asteroidsTitleH;

    SDL_Texture* backgroundTexture;

public:
    RankingScene()
        : btnBack(nullptr),
        fontTitle(nullptr),
        titleTexture(nullptr), titleW(0), titleH(0),
        spaceTitleTex(nullptr), tanksTitleTex(nullptr), splatTitleTex(nullptr), asteroidsTitleTex(nullptr),
        spaceTitleW(0), spaceTitleH(0), tanksTitleW(0), tanksTitleH(0), splatTitleW(0), splatTitleH(0), asteroidsTitleW(0), asteroidsTitleH(0),
        backgroundTexture(nullptr)
    {
        for (int i = 0; i < 10; i++) {
            strcpy_s(spaceScores[i].name, sizeof(spaceScores[i].name), "---");
            spaceScores[i].score = 0;
            strcpy_s(tanksScores[i].name, sizeof(tanksScores[i].name), "---");
            tanksScores[i].score = 0;
            strcpy_s(splatScores[i].name, sizeof(splatScores[i].name), "---");
            splatScores[i].score = 0;
            strcpy_s(asteroidsScores[i].name, sizeof(asteroidsScores[i].name), "---");
            asteroidsScores[i].score = 0;
        }
    }

    void OnEnter() override {
        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));

        RM.LoadFont("resources/fonts/fuente.otf", 36);
        fontTitle = RM.GetFont("resources/fonts/fuente.otf");

        titleTexture = CreateTextTexture("RANKING", fontTitle, &titleW, &titleH);
        spaceTitleTex = CreateTextTexture("SpaceInv", fontTitle, &spaceTitleW, &spaceTitleH);
        tanksTitleTex = CreateTextTexture("Tanks", fontTitle, &tanksTitleW, &tanksTitleH);
        splatTitleTex = CreateTextTexture("Splat!", fontTitle, &splatTitleW, &splatTitleH);
        asteroidsTitleTex = CreateTextTexture("Asteroids", fontTitle, &asteroidsTitleW, &asteroidsTitleH);

        LoadScoresFromFile();

        RM.LoadFont("resources/fonts/fuente.otf", 28);
        TTF_Font* fontButton = RM.GetFont("resources/fonts/fuente.otf");
        btnBack = new Button("Back", 600, 650, 200, 50, fontButton);
    }

    void OnExit() override {
        SaveScoresToFile();

        if (titleTexture) SDL_DestroyTexture(titleTexture);
        if (spaceTitleTex) SDL_DestroyTexture(spaceTitleTex);
        if (tanksTitleTex) SDL_DestroyTexture(tanksTitleTex);
        if (splatTitleTex) SDL_DestroyTexture(splatTitleTex);
        if (asteroidsTitleTex) SDL_DestroyTexture(asteroidsTitleTex);
        if (btnBack) {
            delete btnBack;
            btnBack = nullptr;
        }
        Scene::OnExit();
    }

    void Update() override {
        btnBack->Update();
        if (btnBack->IsClicked()) {
            SM.SetNextScene("MainMenu");
        }
    }

    void Render() override {
        SDL_RenderCopy(RM.GetRenderer(), backgroundTexture, nullptr, nullptr);

        RenderTexture(titleTexture, (1360 - titleW) / 2, 20, titleW, titleH);

        int col1x = 100;
        int col2x = 420;
        int col3x = 740;
        int col4x = 1060;
        int topY = 100;

        RenderTexture(spaceTitleTex, col1x, topY, spaceTitleW, spaceTitleH);
        RenderTexture(tanksTitleTex, col2x, topY, tanksTitleW, tanksTitleH);
        RenderTexture(splatTitleTex, col3x, topY, splatTitleW, splatTitleH);
        RenderTexture(asteroidsTitleTex, col4x, topY, asteroidsTitleW, asteroidsTitleH);

        int rowY = topY + 60;
        for (int i = 0; i < 10; i++) {
            RenderScore(spaceScores[i], col1x, rowY + i * 40);
            RenderScore(tanksScores[i], col2x, rowY + i * 40);
            RenderScore(splatScores[i], col3x, rowY + i * 40);
            RenderScore(asteroidsScores[i], col4x, rowY + i * 40);
        }

        btnBack->Render();
    }

    void AddScoreToRanking(int mode, const std::string& name, int newScore) {
        if (newScore <= 0) return;
        ScoreRecord* arr = nullptr;
        if (mode == 0) arr = spaceScores;
        else if (mode == 1) arr = tanksScores;
        else if (mode == 2) arr = splatScores;
        else if (mode == 3) arr = asteroidsScores;

        std::vector<ScoreRecord> temp(arr, arr + 10);
        ScoreRecord newRecord;
        std::string finalName = name.empty() ? "Player" : name;
        strncpy_s(newRecord.name, sizeof(newRecord.name), finalName.c_str(), _TRUNCATE);
        newRecord.score = newScore;
        temp.push_back(newRecord);

        std::sort(temp.begin(), temp.end(), [](const ScoreRecord& a, const ScoreRecord& b) {
            return a.score > b.score;
            });

        for (int i = 0; i < 10; i++) {
            arr[i] = temp[i];
        }
    }

    static void InsertScoreForMode(int mode, const std::string& name, int score) {
        RankingScene* ranking = dynamic_cast<RankingScene*>(SM.GetScene("RankingScene"));
        if (ranking) {
            ranking->AddScoreToRanking(mode, name, score);
            ranking->SaveScoresToFile();
        }
    }

private:
    void LoadScoresFromFile() {
        std::ifstream file("resources/highscores_v3.bin", std::ios::binary);
        if (!file.is_open()) {
            std::ifstream fileOld("resources/highscores_v2.bin", std::ios::binary);
            if (fileOld.is_open()) {
                fileOld.read(reinterpret_cast<char*>(spaceScores), sizeof(spaceScores));
                fileOld.read(reinterpret_cast<char*>(tanksScores), sizeof(tanksScores));
                fileOld.read(reinterpret_cast<char*>(splatScores), sizeof(splatScores));
                fileOld.close();
            }
            return;
        }
        file.read(reinterpret_cast<char*>(spaceScores), sizeof(spaceScores));
        file.read(reinterpret_cast<char*>(tanksScores), sizeof(tanksScores));
        file.read(reinterpret_cast<char*>(splatScores), sizeof(splatScores));
        file.read(reinterpret_cast<char*>(asteroidsScores), sizeof(asteroidsScores));
        file.close();
    }

    void SaveScoresToFile() {
        std::ofstream file("resources/highscores_v3.bin", std::ios::binary | std::ios::trunc);
        if (!file.is_open()) return;
        file.write(reinterpret_cast<char*>(spaceScores), sizeof(spaceScores));
        file.write(reinterpret_cast<char*>(tanksScores), sizeof(tanksScores));
        file.write(reinterpret_cast<char*>(splatScores), sizeof(splatScores));
        file.write(reinterpret_cast<char*>(asteroidsScores), sizeof(asteroidsScores));
        file.close();
    }

    SDL_Texture* CreateTextTexture(const std::string& text, TTF_Font* font, int* outW, int* outH) {
        if (!font) return nullptr;
        SDL_Color color = { 0, 0, 0, 255 };
        SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surf) return nullptr;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(RM.GetRenderer(), surf);
        if (outW) *outW = surf->w;
        if (outH) *outH = surf->h;
        SDL_FreeSurface(surf);
        return tex;
    }

    void RenderTexture(SDL_Texture* tex, int x, int y, int w, int h, bool useCenter = false) {
        if (!tex) return;
        int tw, th;
        SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);
        if (w == 0) w = tw;
        if (h == 0) h = th;
        SDL_Rect dst;
        dst.w = w;
        dst.h = h;
        dst.x = useCenter ? x - w / 2 : x;
        dst.y = y;
        SDL_RenderCopy(RM.GetRenderer(), tex, nullptr, &dst);
    }

    void RenderScore(const ScoreRecord& record, int x, int y) {
        char buffer[64];
        sprintf_s(buffer, "%s - %d", record.name, record.score);
        SDL_Texture* scoreTex = CreateTextTexture(buffer, fontTitle, nullptr, nullptr);
        RenderTexture(scoreTex, x, y, 0, 0, true);
        if (scoreTex) SDL_DestroyTexture(scoreTex);
    }
};