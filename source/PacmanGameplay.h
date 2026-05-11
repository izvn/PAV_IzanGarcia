#pragma once
#include "Scene.h"
#include "PacmanPlayer.h"
#include "PacmanGhost.h"
#include "PacmanWall.h"
#include "PacmanDot.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "GameConfig.h"
#include "TextObject.h"
#include "RankingScene.h"
#include "SceneManager.h"
#include <cstdlib>
#include <vector>

class PacmanGameplay : public Scene {
private:
    PacmanPlayer* player;
    std::vector<PacmanWall*> walls;
    std::vector<PacmanDot*> dots;
    std::vector<PacmanGhost*> ghosts;

    bool gameOver;
    int score;
    int highScore;
    TextObject* scoreText;
    TextObject* livesText;
    TextObject* highScoreText;
    bool paused;

    bool showingWaveCompleted;
    float waveCompletedTimer;
    SDL_Texture* waveCompletedTex;

public:
    PacmanGameplay() : Scene(), player(nullptr), gameOver(false), score(0), highScore(0),
        scoreText(nullptr), livesText(nullptr), highScoreText(nullptr), paused(false),
        showingWaveCompleted(false), waveCompletedTimer(0.0f), waveCompletedTex(nullptr) {
    }

    void ForceCleanup() {
        paused = false;
        Scene::OnExit();
    }

    void OnEnter() override {
        if (paused) {
            paused = false;
            return;
        }
        gameOver = false;
        score = 0;
        showingWaveCompleted = false;

        RM.LoadTexture("resources/wavecompleted.png");
        waveCompletedTex = RM.GetTexture("resources/wavecompleted.png");

        AM.StopMusic();
        AM.PlaySong("menu_music");

        scoreText = new TextObject("", Vector2(0, 0), Vector2(150, 40), "SCORE: 000000");
        scoreText->GetTransform()->position = Vector2(120, 20);
        SPAWN.SpawnObject(scoreText);

        livesText = new TextObject("", Vector2(0, 0), Vector2(150, 40), "Lives: 3");
        livesText->GetTransform()->position = Vector2(680, 20);
        SPAWN.SpawnObject(livesText);

        highScoreText = new TextObject("", Vector2(0, 0), Vector2(200, 40), "High: 000000");
        highScoreText->GetTransform()->position = Vector2(1260, 20);
        SPAWN.SpawnObject(highScoreText);

        LoadMap();
    }

    void OnExit() override {
        if (SM.GetNextSceneName() == "PacmanPause") {
            paused = true;
            return;
        }
        AM.StopMusic();
        Scene::OnExit();
    }

    void Update() override {
        if (!gameOver && Input.GetEvent(SDLK_ESCAPE, DOWN)) {
            SM.SetNextScene("PacmanPause");
            return;
        }

        if (showingWaveCompleted) {
            waveCompletedTimer -= TIME.GetDeltaTime();
            if (waveCompletedTimer <= 0) {
                showingWaveCompleted = false;
                LoadMap();
            }
            return;
        }

        UpdateHUD();

        float dt = TIME.GetDeltaTime();

        if (player) {
            Vector2 pPos = player->GetTransform()->position;
            Vector2 pSize = player->GetTransform()->size;

            Vector2 tryPos = pPos + player->desiredDir * player->speed * dt;
            if (!IsOverlappingWall(tryPos, pSize)) {
                player->currentDir = player->desiredDir;
            }

            Vector2 nextPos = pPos + player->currentDir * player->speed * dt;
            if (!IsOverlappingWall(nextPos, pSize)) {
                player->GetTransform()->position = nextPos;
            }

            if (player->GetTransform()->position.x < -20) player->GetTransform()->position.x = RM.WINDOW_WIDTH + 20;
            if (player->GetTransform()->position.x > RM.WINDOW_WIDTH + 20) player->GetTransform()->position.x = -20;
        }

        for (auto* g : ghosts) {
            Vector2 gPos = g->GetTransform()->position;
            Vector2 gSize = g->GetTransform()->size;

            int ix = (int)gPos.x;
            int iy = (int)gPos.y;
            if (ix % 40 > 18 && ix % 40 < 22 && iy % 40 > 18 && iy % 40 < 22) {
                if (rand() % 100 < 2) {
                    std::vector<Vector2> sideDirs;
                    if (g->currentDir.x != 0) { sideDirs.push_back(Vector2(0, 1)); sideDirs.push_back(Vector2(0, -1)); }
                    else { sideDirs.push_back(Vector2(1, 0)); sideDirs.push_back(Vector2(-1, 0)); }

                    for (auto d : sideDirs) {
                        if (!IsOverlappingWall(gPos + d * 25.0f, gSize)) {
                            g->currentDir = d;
                            break;
                        }
                    }
                }
            }

            Vector2 nextPos = gPos + g->currentDir * g->speed * dt;
            if (IsOverlappingWall(nextPos, gSize)) {
                std::vector<Vector2> dirs = { Vector2(1,0), Vector2(-1,0), Vector2(0,1), Vector2(0,-1) };
                std::vector<Vector2> validDirs;
                for (auto d : dirs) {
                    if (d.x == -g->currentDir.x && d.y == -g->currentDir.y) continue;
                    if (!IsOverlappingWall(gPos + d * g->speed * dt * 3.0f, gSize)) {
                        validDirs.push_back(d);
                    }
                }
                if (!validDirs.empty()) g->currentDir = validDirs[rand() % validDirs.size()];
                else g->currentDir = Vector2(-g->currentDir.x, -g->currentDir.y);
            }
            else {
                g->GetTransform()->position = nextPos;
            }

            if (g->GetTransform()->position.x < -20) g->GetTransform()->position.x = RM.WINDOW_WIDTH + 20;
            if (g->GetTransform()->position.x > RM.WINDOW_WIDTH + 20) g->GetTransform()->position.x = -20;
        }

        if (player) {
            for (int i = (int)dots.size() - 1; i >= 0; i--) {
                if (player->GetRigidbody()->CheckCollision(dots[i]->GetRigidbody())) {
                    dots[i]->Destroy();
                    dots.erase(dots.begin() + i);
                    AddScore(10);
                }
            }

            for (auto* g : ghosts) {
                if (player->GetRigidbody()->CheckCollision(g->GetRigidbody())) {
                    player->LoseLife();
                    AM.PlayClip("splat_player_hit", 0);
                    ResetPositions();
                    break;
                }
            }
        }

        for (int i = (int)objects.size() - 1; i >= 0; i--) {
            if (objects[i]->IsPendingDestroy()) {
                delete objects[i];
                objects.erase(objects.begin() + i);
            }
        }

        while (SPAWN.GetSpawnedObjectsCount() > 0) {
            objects.push_back(SPAWN.GetSpawnedObject());
        }

        for (Object* o : objects) {
            o->Update();
        }

        if (player && !player->IsAlive() && !gameOver) {
            EndGame();
        }

        if (!gameOver && dots.empty() && !showingWaveCompleted) {
            AM.PlayClip("tank_end_wave", 0);
            showingWaveCompleted = true;
            waveCompletedTimer = 2.0f;
        }
    }

    void Render() override {
        for (Object* o : objects) {
            o->Render();
        }

        if (showingWaveCompleted && waveCompletedTex) {
            SDL_Rect dest = { 0, 0, (int)RM.WINDOW_WIDTH, (int)RM.WINDOW_HEIGHT };
            SDL_RenderCopy(RM.GetRenderer(), waveCompletedTex, nullptr, &dest);
        }
    }

private:
    bool IsOverlappingWall(Vector2 pos, Vector2 size) {
        float l1 = pos.x - size.x / 2;
        float r1 = pos.x + size.x / 2;
        float t1 = pos.y - size.y / 2;
        float b1 = pos.y + size.y / 2;
        for (auto* w : walls) {
            float l2 = w->GetTransform()->position.x - w->GetTransform()->size.x / 2;
            float r2 = w->GetTransform()->position.x + w->GetTransform()->size.x / 2;
            float t2 = w->GetTransform()->position.y - w->GetTransform()->size.y / 2;
            float b2 = w->GetTransform()->position.y + w->GetTransform()->size.y / 2;
            if (l1 < r2 && r1 > l2 && t1 < b2 && b1 > t2) return true;
        }
        return false;
    }

    void ResetPositions() {
        if (player) {
            player->GetTransform()->position = player->spawnPos;
            player->currentDir = Vector2(0, 0);
            player->desiredDir = Vector2(0, 0);
        }
        for (auto* g : ghosts) {
            g->GetTransform()->position = g->spawnPos;
            g->currentDir = Vector2(1, 0);
        }
    }

    void LoadMap() {
        for (Object* o : objects) {
            if (!dynamic_cast<TextObject*>(o)) {
                o->Destroy();
            }
        }
        walls.clear();
        dots.clear();
        ghosts.clear();

        const int ROWS = 18;
        const int COLS = 34;
        int maze[ROWS][COLS] = {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1},
            {1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
            {1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,3,1,1,3,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1},
            {3,3,3,3,3,1,0,1,1,3,3,3,3,3,3,3,5,5,3,3,3,3,3,3,3,1,1,0,1,3,3,3,3,3},
            {1,1,1,1,1,1,0,1,1,3,1,1,1,1,3,3,3,3,3,3,1,1,1,1,3,1,1,0,1,1,1,1,1,1},
            {3,3,3,3,3,3,0,3,3,3,1,3,3,3,3,5,5,3,3,3,1,3,3,3,0,3,3,3,3,3,3,3,3,3},
            {1,1,1,1,1,1,0,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,1,0,1,1,1,1,1,1},
            {3,3,3,3,3,1,0,1,1,3,3,3,3,3,3,3,4,3,3,3,3,3,3,3,3,1,1,0,1,3,3,3,3,3},
            {1,1,1,1,1,1,0,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,1,0,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1},
            {1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
        };

        float tileSize = 40.0f;
        float offsetX = 0.0f;
        float offsetY = 48.0f;

        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                Vector2 pos(offsetX + c * tileSize + tileSize / 2, offsetY + r * tileSize + tileSize / 2);
                int val = maze[r][c];
                if (val == 1) {
                    PacmanWall* w = new PacmanWall(pos, Vector2(tileSize, tileSize));
                    SPAWN.SpawnObject(w);
                    walls.push_back(w);
                }
                else if (val == 0) {
                    PacmanDot* d = new PacmanDot(pos);
                    SPAWN.SpawnObject(d);
                    dots.push_back(d);
                }
                else if (val == 4) {
                    player = new PacmanPlayer(pos);
                    SPAWN.SpawnObject(player);
                }
                else if (val == 5) {
                    PacmanGhost* g = new PacmanGhost(pos);
                    SPAWN.SpawnObject(g);
                    ghosts.push_back(g);
                }
            }
        }
    }

    void AddScore(int amount) {
        score += amount;
        if (score > highScore) highScore = score;
    }

    void UpdateHUD() {
        if (scoreText) {
            char buffer[64];
            sprintf_s(buffer, "SCORE: %06d", score);
            scoreText->SetText(buffer);
        }
        if (livesText && player) {
            char buffer[64];
            sprintf_s(buffer, "Lives: %d", player->GetLives());
            livesText->SetText(buffer);
        }
        if (highScoreText) {
            char buffer[64];
            sprintf_s(buffer, "High: %06d", highScore);
            highScoreText->SetText(buffer);
        }
    }

    void EndGame() {
        if (player) score += player->GetLives() * 5000;
        gameOver = true;
        GameConfig::pendingMode = 11;
        GameConfig::pendingScore = score;
        SM.SetNextScene("GameOver");
    }
};