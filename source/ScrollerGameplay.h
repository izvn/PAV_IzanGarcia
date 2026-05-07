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
    int currentWave;
    float waveTimer;
    SDL_Texture* backgroundTexture;
    float bgX1;
    float bgX2;
    int score;
    int highScore;
    TextObject* scoreText;
    TextObject* livesText;
    TextObject* waveText;
    TextObject* highScoreText;
    bool paused;
    bool showingWaveCompleted;
    float waveCompletedTimer;
    SDL_Texture* waveCompletedTex;

public:
    ScrollerGameplay()
        : Scene(), player(nullptr), gameOver(false), spawnTimer(0), spawnInterval(1.5f),
        currentWave(1), waveTimer(0), backgroundTexture(nullptr), bgX1(0), bgX2(1360),
        score(0), highScore(0), scoreText(nullptr), livesText(nullptr), waveText(nullptr),
        highScoreText(nullptr), paused(false),
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
        currentWave = 1;
        spawnInterval = 1.5f;
        spawnTimer = 1.0f;
        waveTimer = 0.0f;
        bgX1 = 0;
        bgX2 = (float)RM.WINDOW_WIDTH;
        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));
        RM.LoadTexture("resources/wavecompleted.png");
        waveCompletedTex = RM.GetTexture("resources/wavecompleted.png");
        AM.StopMusic();
        AM.PlaySong("menu_music");
        srand((unsigned)time(NULL));
        player = new ScrollerPlayer();
        SPAWN.SpawnObject(player);
        scoreText = new TextObject("", Vector2(0, 0), Vector2(150, 40), "SCORE: 000000");
        scoreText->GetTransform()->position = Vector2(120, 30);
        SPAWN.SpawnObject(scoreText);
        livesText = new TextObject("", Vector2(0, 0), Vector2(150, 40), "Lives: 3");
        livesText->GetTransform()->position = Vector2(500, 30);
        SPAWN.SpawnObject(livesText);
        waveText = new TextObject("", Vector2(0, 0), Vector2(150, 40), "WAVE: 1");
        waveText->GetTransform()->position = Vector2(880, 30);
        SPAWN.SpawnObject(waveText);
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
        if (showingWaveCompleted) {
            waveCompletedTimer -= TIME.GetDeltaTime();
            if (waveCompletedTimer <= 0) {
                showingWaveCompleted = false;
            }
            return;
        }
        UpdateHUD();
        UpdateBackground();
        if (!gameOver) {
            float dt = TIME.GetDeltaTime();
            waveTimer += dt;
            if (waveTimer >= 15.0f) {
                waveTimer = 0.0f;
                currentWave++;
                spawnInterval = std::fmax(0.3f, spawnInterval - 0.15f);
                AM.PlayClip("tank_end_wave", 0);
                ClearProjectiles();
                showingWaveCompleted = true;
                waveCompletedTimer = 2.0f;
            }
            spawnTimer -= dt;
            if (spawnTimer <= 0.0f) {
                int enemiesToSpawn = 1 + (currentWave / 2);
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
            SDL_Rect dest1 = { (int)bgX1, 0, (int)RM.WINDOW_WIDTH, (int)RM.WINDOW_HEIGHT };
            SDL_Rect dest2 = { (int)bgX2, 0, (int)RM.WINDOW_WIDTH, (int)RM.WINDOW_HEIGHT };
            SDL_RenderCopy(RM.GetRenderer(), backgroundTexture, nullptr, &dest1);
            SDL_RenderCopy(RM.GetRenderer(), backgroundTexture, nullptr, &dest2);
        }
        for (Object* o : objects) {
            o->Render();
        }
        if (showingWaveCompleted && waveCompletedTex) {
            SDL_Rect dest = { 0, 0, (int)RM.WINDOW_WIDTH, (int)RM.WINDOW_HEIGHT };
            SDL_RenderCopy(RM.GetRenderer(), waveCompletedTex, nullptr, &dest);
        }
    }

private:
    void ClearProjectiles() {
        for (Object* o : objects) {
            if (dynamic_cast<Bullet*>(o)) o->Destroy();
        }
    }

    void UpdateBackground() {
        float speed = 100.0f * TIME.GetDeltaTime();
        bgX1 -= speed;
        bgX2 -= speed;
        if (bgX1 <= -(float)RM.WINDOW_WIDTH) bgX1 = bgX2 + (float)RM.WINDOW_WIDTH;
        if (bgX2 <= -(float)RM.WINDOW_WIDTH) bgX2 = bgX1 + (float)RM.WINDOW_WIDTH;
    }

    void SpawnEnemy() {
        float yPos = 80.0f + (rand() % (RM.WINDOW_HEIGHT - 160));
        Vector2 pos((float)RM.WINDOW_WIDTH + 50.0f, yPos);
        float speed = 150.0f + (currentWave * 15.0f) + (rand() % 50);
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
        if (waveText) {
            char buffer[64];
            sprintf_s(buffer, "WAVE: %d", currentWave);
            waveText->SetText(buffer);
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