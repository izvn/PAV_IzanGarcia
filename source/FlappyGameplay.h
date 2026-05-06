#pragma once
#include "Scene.h"
#include "FlappyPlayer.h"
#include "FlappyObstacle.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "GameConfig.h"
#include "TextObject.h"
#include "RankingScene.h"
#include "SceneManager.h"
#include <cstdlib>
#include <vector>
#include <cmath>

class FlappyGameplay : public Scene {
private:
    FlappyPlayer* player;
    bool gameOver;

    float spawnTimer;
    float spawnInterval;
    float pipeSpeed;

    SDL_Texture* backgroundTexture;

    int score;
    int highScore;
    TextObject* scoreText;
    TextObject* highScoreText;

    bool paused;

public:
    FlappyGameplay()
        : Scene(), player(nullptr), gameOver(false), spawnTimer(0), spawnInterval(2.0f), pipeSpeed(250.0f),
        backgroundTexture(nullptr), score(0), highScore(0), scoreText(nullptr), highScoreText(nullptr), paused(false) {
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
        pipeSpeed = 250.0f;
        spawnInterval = 2.0f;
        spawnTimer = 1.0f;

        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));
        AM.StopMusic();
        AM.PlaySong("menu_music");

        srand((unsigned)time(NULL));

        player = new FlappyPlayer();
        SPAWN.SpawnObject(player);

        scoreText = new TextObject("", Vector2(0, 0), Vector2(150, 40), "SCORE: 000000");
        scoreText->GetTransform()->position = Vector2(120, 30);
        SPAWN.SpawnObject(scoreText);

        highScoreText = new TextObject("", Vector2(0, 0), Vector2(200, 40), "High: 000000");
        highScoreText->GetTransform()->position = Vector2(1260, 30);
        SPAWN.SpawnObject(highScoreText);
    }

    void OnExit() override {
        if (SM.GetNextSceneName() == "FlappyPause") {
            paused = true;
            return;
        }
        AM.StopMusic();
        Scene::OnExit();
    }

    void Update() override {
        if (!gameOver && Input.GetEvent(SDLK_ESCAPE, DOWN)) {
            SM.SetNextScene("FlappyPause");
            return;
        }

        UpdateHUD();

        if (player && player->HasStarted()) {
            if (!gameOver) {
                float dt = TIME.GetDeltaTime();
                spawnTimer -= dt;
                if (spawnTimer <= 0.0f) {
                    SpawnPipes();
                    spawnTimer = spawnInterval;
                    pipeSpeed += 5.0f;
                    spawnInterval = std::fmax(1.0f, spawnInterval - 0.02f);
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
            if (player && !gameOver && player->HasStarted()) {
                if (FlappyObstacle* obs = dynamic_cast<FlappyObstacle*>(o)) {
                    if (obs->CheckPassed(player->GetTransform()->position.x)) {
                        AddScore(100);
                        AM.PlayClip("space_invaders_end_wave", 0);
                    }
                }
            }
        }

        if (player && player->HasStarted()) {
            CheckCollisions();
        }

        if (player && !player->IsAlive() && !gameOver) {
            EndGame();
        }
    }

    void Render() override {
        if (backgroundTexture) {
            SDL_RenderCopy(RM.GetRenderer(), backgroundTexture, nullptr, nullptr);
        }
        for (Object* o : objects) {
            o->Render();
        }
    }

private:
    void SpawnPipes() {
        float blockSize = 48.0f;
        int totalBlocks = 17;
        int gapSizeBlocks = 5;

        int minGapStart = 2;
        int maxGapStart = totalBlocks - gapSizeBlocks - 2;
        int gapStart = minGapStart + (rand() % (maxGapStart - minGapStart + 1));

        float startX = (float)RM.WINDOW_WIDTH + 50.0f;
        bool triggerAssigned = false;

        for (int i = 0; i < totalBlocks; i++) {
            if (i >= gapStart && i < gapStart + gapSizeBlocks) {
                continue;
            }

            bool isTrigger = false;
            if (!triggerAssigned && i >= gapStart + gapSizeBlocks) {
                isTrigger = true;
                triggerAssigned = true;
            }

            float rot = (i < gapStart) ? 180.0f : 0.0f;
            Vector2 pos(startX, i * blockSize);

            SPAWN.SpawnObject(new FlappyObstacle(pos, pipeSpeed, isTrigger, rot));
        }
    }

    void CheckCollisions() {
        if (!player) return;
        for (Object* o : objects) {
            if (Enemy* enemy = dynamic_cast<Enemy*>(o)) {
                if (player->GetRigidbody()->CheckCollision(enemy->GetRigidbody())) {
                    player->OnCollisionEnter(enemy);
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
        if (highScoreText) {
            char buffer[64];
            sprintf_s(buffer, "High: %06d", highScore);
            highScoreText->SetText(buffer);
        }
    }

    void EndGame() {
        gameOver = true;
        GameConfig::pendingMode = 8;
        GameConfig::pendingScore = score;
        SM.SetNextScene("GameOver");
    }
};