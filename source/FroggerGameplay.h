#pragma once
#include "Scene.h"
#include "FroggerPlayer.h"
#include "FroggerEnemy.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "GameConfig.h"
#include "TextObject.h"
#include "RankingScene.h"
#include "SceneManager.h"
#include <cstdlib>
#include <vector>

class FroggerGameplay : public Scene {
private:
    FroggerPlayer* player;
    bool gameOver;

    int currentLevel;
    SDL_Texture* backgroundTexture;
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
    FroggerGameplay()
        : Scene(), player(nullptr), gameOver(false), currentLevel(1),
        backgroundTexture(nullptr), score(0), highScore(0),
        scoreText(nullptr), livesText(nullptr), highScoreText(nullptr), paused(false),
        showingWaveCompleted(false), waveCompletedTimer(0.0f), waveCompletedTex(nullptr) {
    }

    void ForceCleanup() {
        paused = false;
        showingWaveCompleted = false;
        Scene::OnExit();
    }

    void OnEnter() override {
        if (paused) {
            paused = false;
            return;
        }

        gameOver = false;
        showingWaveCompleted = false;
        score = 0;
        currentLevel = 1;

        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));

        RM.LoadTexture("resources/wavecompleted.png");
        waveCompletedTex = RM.GetTexture("resources/wavecompleted.png");

        AM.StopMusic();
        AM.PlaySong("menu_music");

        srand((unsigned)time(NULL));

        player = new FroggerPlayer();
        SPAWN.SpawnObject(player);

        scoreText = new TextObject("", Vector2(0, 0), Vector2(150, 40), "SCORE: 000000");
        scoreText->GetTransform()->position = Vector2(120, 30);
        SPAWN.SpawnObject(scoreText);

        livesText = new TextObject("", Vector2(0, 0), Vector2(150, 40), "Lives: 3");
        livesText->GetTransform()->position = Vector2(680, 30);
        SPAWN.SpawnObject(livesText);

        highScoreText = new TextObject("", Vector2(0, 0), Vector2(200, 40), "High: 000000");
        highScoreText->GetTransform()->position = Vector2(1260, 30);
        SPAWN.SpawnObject(highScoreText);

        SpawnLevel(currentLevel);
    }

    void OnExit() override {
        if (SM.GetNextSceneName() == "FroggerPause") {
            paused = true;
            return;
        }
        AM.StopMusic();
        Scene::OnExit();
    }

    void Update() override {
        if (!gameOver && Input.GetEvent(SDLK_ESCAPE, DOWN)) {
            SM.SetNextScene("FroggerPause");
            return;
        }

        if (showingWaveCompleted) {
            waveCompletedTimer -= TIME.GetDeltaTime();
            if (waveCompletedTimer <= 0) {
                showingWaveCompleted = false;
                SpawnLevel(currentLevel);
            }
            return;
        }

        UpdateHUD();

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

        CheckCollisions();

        if (player && !player->IsAlive() && !gameOver) {
            EndGame();
        }

        if (player && player->GetTransform()->position.y <= 90.0f) {
            AddScore(500 * currentLevel);
            currentLevel++;
            player->ResetPosition();
            AM.PlayClip("tank_end_wave", 0);

            ClearEnemiesSilently();

            showingWaveCompleted = true;
            waveCompletedTimer = 2.0f;
        }
    }

    void Render() override {
        if (backgroundTexture) {
            SDL_RenderCopy(RM.GetRenderer(), backgroundTexture, nullptr, nullptr);
        }
        for (Object* o : objects) {
            o->Render();
        }

        if (showingWaveCompleted && waveCompletedTex) {
            int texW, texH;
            SDL_QueryTexture(waveCompletedTex, NULL, NULL, &texW, &texH);
            SDL_Rect dest = {
                (int)((RM.WINDOW_WIDTH - texW) / 2),
                (int)((RM.WINDOW_HEIGHT - texH) / 2),
                texW, texH
            };
            SDL_RenderCopy(RM.GetRenderer(), waveCompletedTex, nullptr, &dest);
        }
    }

private:
    void SpawnLevel(int level) {
        float baseY = RM.WINDOW_HEIGHT - 112.0f;
        int numLanes = 9;

        for (int i = 0; i < numLanes; i++) {
            if (i == 4) continue;

            float laneY = baseY - (i * 64.0f);
            float direction = (i % 2 == 0) ? 1.0f : -1.0f;
            float baseSpeed = 90.0f + (rand() % 40) + (level * 15.0f);
            int numEnemies = 2 + (rand() % 3);

            float spacing = RM.WINDOW_WIDTH / numEnemies;
            float offset = (float)(rand() % 150);

            for (int j = 0; j < numEnemies; j++) {
                Vector2 pos(offset + j * spacing, laneY);
                SPAWN.SpawnObject(new FroggerEnemy(pos, baseSpeed, direction));
            }
        }
    }

    void ClearEnemiesSilently() {
        for (Object* o : objects) {
            if (FroggerEnemy* e = dynamic_cast<FroggerEnemy*>(o)) {
                e->DestroySilently();
            }
        }
    }

    void CheckCollisions() {
        if (!player) return;

        for (Object* o : objects) {
            if (FroggerEnemy* enemy = dynamic_cast<FroggerEnemy*>(o)) {
                if (player->GetRigidbody()->CheckCollision(enemy->GetRigidbody())) {
                    player->LoseLife();
                    AM.PlayClip("splat_player_hit", 0);
                    if (player->IsAlive()) {
                        player->ResetPosition();
                    }
                }
            }
        }
    }

    void AddScore(int amount) {
        score += amount;
        if (score > highScore) {
            highScore = score;
        }
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
        gameOver = true;
        GameConfig::pendingMode = 5;
        GameConfig::pendingScore = score;
        SM.SetNextScene("GameOver");
    }
};