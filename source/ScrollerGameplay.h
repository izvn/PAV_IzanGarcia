#pragma once
#include "Scene.h"
#include "ScrollerPlayer.h"
#include "ScrollerEnemy.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "GameConfig.h"
#include "TextObject.h"
#include "RankingScene.h"
#include "SceneManager.h"
#include <cstdlib>
#include <vector>
#include <cmath>

class ScrollerGameplay : public Scene {
private:
    ScrollerPlayer* player;
    bool gameOver;
    float spawnTimer;
    float spawnInterval;
    float difficultyTimer;
    int difficultyLevel;
    SDL_Texture* backgroundTexture;
    int score;
    int highScore;
    TextObject* scoreText;
    TextObject* livesText;
    TextObject* highScoreText;
    bool paused;

public:
    ScrollerGameplay()
        : Scene(), player(nullptr), gameOver(false), spawnTimer(0), spawnInterval(1.5f),
        difficultyTimer(0), difficultyLevel(1), backgroundTexture(nullptr),
        score(0), highScore(0), scoreText(nullptr), livesText(nullptr),
        highScoreText(nullptr), paused(false) {
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
        difficultyLevel = 1;
        spawnInterval = 1.5f;
        spawnTimer = 1.0f;
        difficultyTimer = 0.0f;
        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));
        AM.StopMusic();
        AM.PlaySong("menu_music");
        srand((unsigned)time(NULL));
        player = new ScrollerPlayer();
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
    }

    void OnExit() override {
        if (SM.GetNextSceneName() == "ScrollerPause") {
            paused = true;
            return;
        }
        AM.StopMusic();
        Scene::OnExit();
    }

    void Update() override {
        if (!gameOver && Input.GetEvent(SDLK_ESCAPE, DOWN)) {
            SM.SetNextScene("ScrollerPause");
            return;
        }
        UpdateHUD();

        if (!gameOver) {
            float dt = TIME.GetDeltaTime();
            difficultyTimer += dt;
            if (difficultyTimer >= 15.0f) {
                difficultyTimer = 0.0f;
                difficultyLevel++;
                spawnInterval = std::fmax(0.3f, spawnInterval - 0.15f);
            }
            spawnTimer -= dt;
            if (spawnTimer <= 0.0f) {
                int enemiesToSpawn = 1 + (difficultyLevel / 2);
                for (int i = 0; i < enemiesToSpawn; i++) {
                    SpawnEnemy();
                }
                spawnTimer = spawnInterval;
            }
        }
        for (int i = (int)objects.size() - 1; i >= 0; i--) {
            if (objects[i]->IsPendingDestroy()) {
                if (Enemy* e = dynamic_cast<Enemy*>(objects[i])) {
                    if (e->GetTransform()->position.x > -40.0f) {
                        AddScore(50);
                    }
                }
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
    void SpawnEnemy() {
        float yPos = 80.0f + (rand() % (RM.WINDOW_HEIGHT - 160));
        Vector2 pos((float)RM.WINDOW_WIDTH + 50.0f, yPos);
        float speed = 150.0f + (difficultyLevel * 15.0f) + (rand() % 50);
        float verticalRange = (rand() % 2 == 0) ? (float)(rand() % 150) : 0.0f;
        SPAWN.SpawnObject(new ScrollerEnemy(pos, speed, verticalRange));
    }

    void CheckCollisions() {
        for (int i = 0; i < (int)objects.size(); i++) {
            for (int j = i + 1; j < (int)objects.size(); j++) {
                Object* a = objects[i];
                Object* b = objects[j];
                if (a->GetRigidbody()->CheckCollision(b->GetRigidbody())) {
                    a->OnCollisionEnter(b);
                    b->OnCollisionEnter(a);
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
        GameConfig::pendingMode = 7;
        GameConfig::pendingScore = score;
        SM.SetNextScene("GameOver");
    }
};