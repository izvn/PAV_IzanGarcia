#pragma once
#include "Scene.h"
#include <SDL.h>
#include "RenderManager.h"
#include "GameConfig.h"
#include "Button.h"
#include "SceneManager.h"
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
    ScoreRecord galacticScores[10];
    ScoreRecord centipedeScores[10];
    ScoreRecord vortexScores[10];

    Button* btnBack;
    TTF_Font* fontTitle;
    TTF_Font* fontScores;
    SDL_Texture* titleTexture;
    int titleW, titleH;

    struct GameHeader {
        SDL_Texture* tex;
        int w, h;
    } headers[12];

    SDL_Texture* backgroundTexture;

public:
    RankingScene()
        : btnBack(nullptr), fontTitle(nullptr), fontScores(nullptr), titleTexture(nullptr), titleW(0), titleH(0),
        backgroundTexture(nullptr)
    {
        for (int i = 0; i < 12; i++) headers[i] = { nullptr, 0, 0 };
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
            strcpy_s(galacticScores[i].name, sizeof(galacticScores[i].name), "---"); galacticScores[i].score = 0;
            strcpy_s(centipedeScores[i].name, sizeof(centipedeScores[i].name), "---"); centipedeScores[i].score = 0;
            strcpy_s(vortexScores[i].name, sizeof(vortexScores[i].name), "---"); vortexScores[i].score = 0;
        }
    }

    void OnEnter() override {
        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));

        RM.LoadFont("resources/fonts/fuente.otf", 36);
        fontTitle = RM.GetFont("resources/fonts/fuente.otf");
        RM.LoadFont("resources/fonts/fuente.otf", 18);
        fontScores = RM.GetFont("resources/fonts/fuente.otf");

        titleTexture = CreateTextTexture("HALL OF FAME", fontTitle, &titleW, &titleH);

        headers[0].tex = CreateTextTexture("SPACE INVADERS", fontScores, &headers[0].w, &headers[0].h);
        headers[1].tex = CreateTextTexture("TANKS", fontScores, &headers[1].w, &headers[1].h);
        headers[2].tex = CreateTextTexture("SPLAT!", fontScores, &headers[2].w, &headers[2].h);
        headers[3].tex = CreateTextTexture("ASTEROIDS", fontScores, &headers[3].w, &headers[3].h);
        headers[4].tex = CreateTextTexture("BREAKOUT", fontScores, &headers[4].w, &headers[4].h);
        headers[5].tex = CreateTextTexture("FROGGER", fontScores, &headers[5].w, &headers[5].h);
        headers[6].tex = CreateTextTexture("SPACE ARENA", fontScores, &headers[6].w, &headers[6].h);
        headers[7].tex = CreateTextTexture("SCROLLER", fontScores, &headers[7].w, &headers[7].h);
        headers[8].tex = CreateTextTexture("FLAPPY SHIP", fontScores, &headers[8].w, &headers[8].h);
        headers[9].tex = CreateTextTexture("GALACTIC", fontScores, &headers[9].w, &headers[9].h);
        headers[10].tex = CreateTextTexture("CENTIPEDE", fontScores, &headers[10].w, &headers[10].h);
        headers[11].tex = CreateTextTexture("VORTEX", fontScores, &headers[11].w, &headers[11].h);

        LoadScoresFromFile();

        RM.LoadFont("resources/fonts/fuente.otf", 24);
        btnBack = new Button("Back to Menu", (RM.WINDOW_WIDTH - 220) / 2, 680, 220, 45, RM.GetFont("resources/fonts/fuente.otf"));
    }

    void OnExit() override {
        SaveScoresToFile();
        if (titleTexture) SDL_DestroyTexture(titleTexture);
        for (int i = 0; i < 12; i++) {
            if (headers[i].tex) SDL_DestroyTexture(headers[i].tex);
        }
        if (btnBack) { delete btnBack; btnBack = nullptr; }
        Scene::OnExit();
    }

    void Update() override {
        btnBack->Update();
        if (btnBack->IsClicked()) SM.SetNextScene("MainMenu");
    }

    void Render() override {
        SDL_RenderCopy(RM.GetRenderer(), backgroundTexture, nullptr, nullptr);
        RenderTexture(titleTexture, (RM.WINDOW_WIDTH - titleW) / 2, 20, titleW, titleH);

        // Fila 1 (Juegos del 0 al 5)
        int row1Y = 90;
        int colWidth = RM.WINDOW_WIDTH / 6;
        for (int i = 0; i < 6; i++) {
            int x = i * colWidth + colWidth / 2;
            RenderTexture(headers[i].tex, x, row1Y, 0, 0, true);
            RenderTop5(GetArrayByMode(i), x, row1Y + 45);
        }

        // Fila 2 (Juegos del 6 al 11)
        int row2Y = 380;
        for (int i = 6; i < 12; i++) {
            int x = (i - 6) * colWidth + colWidth / 2;
            RenderTexture(headers[i].tex, x, row2Y, 0, 0, true);
            RenderTop5(GetArrayByMode(i), x, row2Y + 45);
        }

        btnBack->Render();
    }

    static void InsertScoreForMode(int mode, const std::string& name, int score) {
        RankingScene* ranking = dynamic_cast<RankingScene*>(SM.GetScene("RankingScene"));
        if (ranking) {
            ranking->AddScoreToRanking(mode, name, score);
            ranking->SaveScoresToFile();
        }
    }

    void AddScoreToRanking(int mode, const std::string& name, int newScore) {
        if (newScore <= 0) return;
        ScoreRecord* arr = GetArrayByMode(mode);
        if (!arr) return;

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

    void SaveScoresToFile() {
        std::ofstream file("resources/highscores_v12.bin", std::ios::binary | std::ios::trunc);
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
        file.write(reinterpret_cast<char*>(galacticScores), sizeof(galacticScores));
        file.write(reinterpret_cast<char*>(centipedeScores), sizeof(centipedeScores));
        file.write(reinterpret_cast<char*>(vortexScores), sizeof(vortexScores));
        file.close();
    }

private:
    ScoreRecord* GetArrayByMode(int mode) {
        switch (mode) {
        case 0: return spaceScores; case 1: return tanksScores; case 2: return splatScores;
        case 3: return asteroidsScores; case 4: return breakoutScores; case 5: return froggerScores;
        case 6: return arenaScores; case 7: return scrollerScores; case 8: return flappyScores;
        case 9: return galacticScores; case 10: return centipedeScores; case 11: return vortexScores;
        default: return nullptr;
        }
    }

    void RenderTop5(ScoreRecord* arr, int x, int startY) {
        for (int i = 0; i < 5; i++) {
            char buffer[64];
            if (arr[i].score > 0) sprintf_s(buffer, "%d. %s - %d", i + 1, arr[i].name, arr[i].score);
            else sprintf_s(buffer, "%d. ---", i + 1);

            SDL_Texture* tex = CreateTextTexture(buffer, fontScores, nullptr, nullptr);
            RenderTexture(tex, x, startY + i * 35, 0, 0, true);
            if (tex) SDL_DestroyTexture(tex);
        }
    }

    void LoadScoresFromFile() {
        std::ifstream file("resources/highscores_v12.bin", std::ios::binary);
        if (!file.is_open()) {
            // Intentamos cargar la versión anterior si la nueva no existe
            std::ifstream fileOld("resources/highscores_v11.bin", std::ios::binary);
            if (fileOld.is_open()) {
                fileOld.read(reinterpret_cast<char*>(spaceScores), sizeof(spaceScores));
                fileOld.read(reinterpret_cast<char*>(tanksScores), sizeof(tanksScores));
                fileOld.read(reinterpret_cast<char*>(splatScores), sizeof(splatScores));
                fileOld.read(reinterpret_cast<char*>(asteroidsScores), sizeof(asteroidsScores));
                fileOld.read(reinterpret_cast<char*>(breakoutScores), sizeof(breakoutScores));
                fileOld.read(reinterpret_cast<char*>(froggerScores), sizeof(froggerScores));
                fileOld.read(reinterpret_cast<char*>(arenaScores), sizeof(arenaScores));
                fileOld.read(reinterpret_cast<char*>(scrollerScores), sizeof(scrollerScores));
                fileOld.read(reinterpret_cast<char*>(flappyScores), sizeof(flappyScores));
                fileOld.read(reinterpret_cast<char*>(galacticScores), sizeof(galacticScores));
                fileOld.read(reinterpret_cast<char*>(centipedeScores), sizeof(centipedeScores));
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
        file.read(reinterpret_cast<char*>(galacticScores), sizeof(galacticScores));
        file.read(reinterpret_cast<char*>(centipedeScores), sizeof(centipedeScores));
        file.read(reinterpret_cast<char*>(vortexScores), sizeof(vortexScores));
        file.close();
    }

    SDL_Texture* CreateTextTexture(const std::string& text, TTF_Font* font, int* outW, int* outH) {
        if (!font) return nullptr;
        SDL_Color color = { 0, 0, 0, 255 }; // Letras negras para asegurar lectura
        SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surf) return nullptr;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(RM.GetRenderer(), surf);
        if (outW) *outW = surf->w; if (outH) *outH = surf->h;
        SDL_FreeSurface(surf);
        return tex;
    }

    void RenderTexture(SDL_Texture* tex, int x, int y, int w, int h, bool useCenter = false) {
        if (!tex) return;
        int tw, th;
        SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);
        SDL_Rect dst = { useCenter ? x - (w ? w : tw) / 2 : x, y, w ? w : tw, h ? h : th };
        SDL_RenderCopy(RM.GetRenderer(), tex, nullptr, &dst);
    }
};