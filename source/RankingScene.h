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
    ScoreRecord breakoutScores[10];
    ScoreRecord froggerScores[10];
    ScoreRecord arenaScores[10];
    ScoreRecord scrollerScores[10];
    ScoreRecord flappyScores[10];

    Button* btnBack;

    TTF_Font* fontTitle;
    SDL_Texture* titleTexture;
    int titleW, titleH;

    SDL_Texture* spaceTitleTex;
    SDL_Texture* tanksTitleTex;
    SDL_Texture* splatTitleTex;
    SDL_Texture* asteroidsTitleTex;
    SDL_Texture* breakoutTitleTex;
    SDL_Texture* froggerTitleTex;
    SDL_Texture* arenaTitleTex;
    SDL_Texture* scrollerTitleTex;
    SDL_Texture* flappyTitleTex;

    int spaceTitleW, spaceTitleH, tanksTitleW, tanksTitleH;
    int splatTitleW, splatTitleH, asteroidsTitleW, asteroidsTitleH;
    int breakoutTitleW, breakoutTitleH, froggerTitleW, froggerTitleH;
    int arenaTitleW, arenaTitleH, scrollerTitleW, scrollerTitleH;
    int flappyTitleW, flappyTitleH;

    SDL_Texture* backgroundTexture;

public:
    RankingScene()
        : btnBack(nullptr), fontTitle(nullptr), titleTexture(nullptr), titleW(0), titleH(0),
        spaceTitleTex(nullptr), tanksTitleTex(nullptr), splatTitleTex(nullptr), asteroidsTitleTex(nullptr),
        breakoutTitleTex(nullptr), froggerTitleTex(nullptr), arenaTitleTex(nullptr), scrollerTitleTex(nullptr), flappyTitleTex(nullptr),
        backgroundTexture(nullptr)
    {
        for (int i = 0; i < 10; i++) {
            strcpy_s(spaceScores[i].name, sizeof(spaceScores[i].name), "---"); spaceScores[i].score = 0;
            strcpy_s(tanksScores[i].name, sizeof(tanksScores[i].name), "---"); tanksScores[i].score = 0;
            strcpy_s(splatScores[i].name, sizeof(splatScores[i].name), "---"); splatScores[i].score = 0;
            strcpy_s(asteroidsScores[i].name, sizeof(asteroidsScores[i].name), "---"); asteroidsScores[i].score = 0;
            strcpy_s(breakoutScores[i].name, sizeof(breakoutScores[i].name), "---"); breakoutScores[i].score = 0;
            strcpy_s(froggerScores[i].name, sizeof(froggerScores[i].name), "---"); froggerScores[i].score = 0;
            strcpy_s(arenaScores[i].name, sizeof(arenaScores[i].name), "---"); arenaScores[i].score = 0;
            strcpy_s(scrollerScores[i].name, sizeof(scrollerScores[i].name), "---"); scrollerScores[i].score = 0;
            strcpy_s(flappyScores[i].name, sizeof(flappyScores[i].name), "---"); flappyScores[i].score = 0;
        }
    }

    void OnEnter() override {
        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));

        RM.LoadFont("resources/fonts/fuente.otf", 20);
        fontTitle = RM.GetFont("resources/fonts/fuente.otf");

        titleTexture = CreateTextTexture("RANKING", fontTitle, &titleW, &titleH);
        spaceTitleTex = CreateTextTexture("Space", fontTitle, &spaceTitleW, &spaceTitleH);
        tanksTitleTex = CreateTextTexture("Tanks", fontTitle, &tanksTitleW, &tanksTitleH);
        splatTitleTex = CreateTextTexture("Splat", fontTitle, &splatTitleW, &splatTitleH);
        asteroidsTitleTex = CreateTextTexture("Asteroid", fontTitle, &asteroidsTitleW, &asteroidsTitleH);
        breakoutTitleTex = CreateTextTexture("Breakout", fontTitle, &breakoutTitleW, &breakoutTitleH);
        froggerTitleTex = CreateTextTexture("Frogger", fontTitle, &froggerTitleW, &froggerTitleH);
        arenaTitleTex = CreateTextTexture("Arena", fontTitle, &arenaTitleW, &arenaTitleH);
        scrollerTitleTex = CreateTextTexture("Scroller", fontTitle, &scrollerTitleW, &scrollerTitleH);
        flappyTitleTex = CreateTextTexture("Flappy", fontTitle, &flappyTitleW, &flappyTitleH);

        LoadScoresFromFile();

        RM.LoadFont("resources/fonts/fuente.otf", 28);
        TTF_Font* fontButton = RM.GetFont("resources/fonts/fuente.otf");
        btnBack = new Button("Back", (1360 - 200) / 2, 680, 200, 50, fontButton);
    }

    void OnExit() override {
        SaveScoresToFile();

        if (titleTexture) SDL_DestroyTexture(titleTexture);
        if (spaceTitleTex) SDL_DestroyTexture(spaceTitleTex);
        if (tanksTitleTex) SDL_DestroyTexture(tanksTitleTex);
        if (splatTitleTex) SDL_DestroyTexture(splatTitleTex);
        if (asteroidsTitleTex) SDL_DestroyTexture(asteroidsTitleTex);
        if (breakoutTitleTex) SDL_DestroyTexture(breakoutTitleTex);
        if (froggerTitleTex) SDL_DestroyTexture(froggerTitleTex);
        if (arenaTitleTex) SDL_DestroyTexture(arenaTitleTex);
        if (scrollerTitleTex) SDL_DestroyTexture(scrollerTitleTex);
        if (flappyTitleTex) SDL_DestroyTexture(flappyTitleTex);
        if (btnBack) { delete btnBack; btnBack = nullptr; }
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

        RenderTexture(titleTexture, (1360 - titleW) / 2, 10, titleW, titleH);

        int cols[9] = { 20, 160, 300, 440, 580, 720, 860, 1000, 1140 };
        int topY = 70;

        RenderTexture(spaceTitleTex, cols[0], topY, spaceTitleW, spaceTitleH);
        RenderTexture(tanksTitleTex, cols[1], topY, tanksTitleW, tanksTitleH);
        RenderTexture(splatTitleTex, cols[2], topY, splatTitleW, splatTitleH);
        RenderTexture(asteroidsTitleTex, cols[3], topY, asteroidsTitleW, asteroidsTitleH);
        RenderTexture(breakoutTitleTex, cols[4], topY, breakoutTitleW, breakoutTitleH);
        RenderTexture(froggerTitleTex, cols[5], topY, froggerTitleW, froggerTitleH);
        RenderTexture(arenaTitleTex, cols[6], topY, arenaTitleW, arenaTitleH);
        RenderTexture(scrollerTitleTex, cols[7], topY, scrollerTitleW, scrollerTitleH);
        RenderTexture(flappyTitleTex, cols[8], topY, flappyTitleW, flappyTitleH);

        int rowY = topY + 40;
        for (int i = 0; i < 10; i++) {
            RenderScore(spaceScores[i], cols[0], rowY + i * 40);
            RenderScore(tanksScores[i], cols[1], rowY + i * 40);
            RenderScore(splatScores[i], cols[2], rowY + i * 40);
            RenderScore(asteroidsScores[i], cols[3], rowY + i * 40);
            RenderScore(breakoutScores[i], cols[4], rowY + i * 40);
            RenderScore(froggerScores[i], cols[5], rowY + i * 40);
            RenderScore(arenaScores[i], cols[6], rowY + i * 40);
            RenderScore(scrollerScores[i], cols[7], rowY + i * 40);
            RenderScore(flappyScores[i], cols[8], rowY + i * 40);
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
        else if (mode == 4) arr = breakoutScores;
        else if (mode == 5) arr = froggerScores;
        else if (mode == 6) arr = arenaScores;
        else if (mode == 7) arr = scrollerScores;
        else if (mode == 8) arr = flappyScores;

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
        std::ifstream file("resources/highscores_v8.bin", std::ios::binary);
        if (!file.is_open()) {
            std::ifstream fileOld("resources/highscores_v7.bin", std::ios::binary);
            if (fileOld.is_open()) {
                fileOld.read(reinterpret_cast<char*>(spaceScores), sizeof(spaceScores));
                fileOld.read(reinterpret_cast<char*>(tanksScores), sizeof(tanksScores));
                fileOld.read(reinterpret_cast<char*>(splatScores), sizeof(splatScores));
                fileOld.read(reinterpret_cast<char*>(asteroidsScores), sizeof(asteroidsScores));
                fileOld.read(reinterpret_cast<char*>(breakoutScores), sizeof(breakoutScores));
                fileOld.read(reinterpret_cast<char*>(froggerScores), sizeof(froggerScores));
                fileOld.read(reinterpret_cast<char*>(arenaScores), sizeof(arenaScores));
                fileOld.read(reinterpret_cast<char*>(scrollerScores), sizeof(scrollerScores));
                fileOld.close();
            }
            return;
        }
        file.read(reinterpret_cast<char*>(spaceScores), sizeof(spaceScores));
        file.read(reinterpret_cast<char*>(tanksScores), sizeof(tanksScores));
        file.read(reinterpret_cast<char*>(splatScores), sizeof(splatScores));
        file.read(reinterpret_cast<char*>(asteroidsScores), sizeof(asteroidsScores));
        file.read(reinterpret_cast<char*>(breakoutScores), sizeof(breakoutScores));
        file.read(reinterpret_cast<char*>(froggerScores), sizeof(froggerScores));
        file.read(reinterpret_cast<char*>(arenaScores), sizeof(arenaScores));
        file.read(reinterpret_cast<char*>(scrollerScores), sizeof(scrollerScores));
        file.read(reinterpret_cast<char*>(flappyScores), sizeof(flappyScores));
        file.close();
    }

    void SaveScoresToFile() {
        std::ofstream file("resources/highscores_v8.bin", std::ios::binary | std::ios::trunc);
        if (!file.is_open()) return;
        file.write(reinterpret_cast<char*>(spaceScores), sizeof(spaceScores));
        file.write(reinterpret_cast<char*>(tanksScores), sizeof(tanksScores));
        file.write(reinterpret_cast<char*>(splatScores), sizeof(splatScores));
        file.write(reinterpret_cast<char*>(asteroidsScores), sizeof(asteroidsScores));
        file.write(reinterpret_cast<char*>(breakoutScores), sizeof(breakoutScores));
        file.write(reinterpret_cast<char*>(froggerScores), sizeof(froggerScores));
        file.write(reinterpret_cast<char*>(arenaScores), sizeof(arenaScores));
        file.write(reinterpret_cast<char*>(scrollerScores), sizeof(scrollerScores));
        file.write(reinterpret_cast<char*>(flappyScores), sizeof(flappyScores));
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