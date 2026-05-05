#pragma once
#include "Scene.h"
#include "ArenaPlayer.h"
#include "ArenaEnemy.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "GameConfig.h"
#include "TextObject.h"
#include "RankingScene.h"
#include "SceneManager.h"
#include <cstdlib>
#include <vector>

class ArenaGameplay : public Scene {
private:
    ArenaPlayer* player;
    bool gameOver;

    float spawnTimer;
    float spawnInterval;
    int currentWave;
    float waveTimer;

    SDL_Texture* backgroundTexture;
    int score;
    int highScore;
    TextObject* scoreText;
    TextObject* livesText;
    TextObject* highScoreText;
    TextObject* waveText;

    bool paused;

public:
    ArenaGameplay()
        : Scene(), player(nullptr), gameOver(false), spawnTimer(0), spawnInterval(2.0f),
        currentWave(1), waveTimer(0), backgroundTexture(nullptr), score(0), highScore(0),
        scoreText(nullptr), livesText(nullptr), highScoreText(nullptr), waveText(nullptr), paused(false) {
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
        currentWave = 1;
        spawnInterval = 2.0f;
        spawnTimer = 1.0f;
        waveTimer = 0.0f;

        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));
        AM.StopMusic();
        AM.PlaySong("tank_music");

        srand((unsigned)time(NULL));

        player = new ArenaPlayer();
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
        if (SM.GetNextSceneName() == "ArenaPause") {
            paused = true;
            return;
        }
        AM.StopMusic();
        Scene::OnExit();
    }

    void Update() override {
        if (!gameOver && Input.GetEvent(SDLK_ESCAPE, DOWN)) {
            SM.SetNextScene("ArenaPause");
            return;
        }

        UpdateHUD();

        if (!gameOver) {
            float dt = TIME.GetDeltaTime();
            waveTimer += dt;
            if (waveTimer >= 10.0f) {
                waveTimer = 0.0f;
                currentWave++;
                spawnInterval = std::fmax(0.3f, spawnInterval - 0.15f);
                AM.PlayClip("tank_end_wave", 0);
            }

            spawnTimer -= dt;
            if (spawnTimer <= 0.0f) {
                int enemiesToSpawn = 1 + (currentWave / 3);
                for (int i = 0; i < enemiesToSpawn; i++) {
                    SpawnEnemy();
                }
                spawnTimer = spawnInterval;
            }
        }

        for (int i = (int)objects.size() - 1; i >= 0; i--) {
            if (objects[i]->IsPendingDestroy()) {
                if (Enemy* e = dynamic_cast<Enemy*>(objects[i])) {
                    AddScore(50);
                    AM.PlayClip("tank_enemy_death", 0);
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
        int edge = rand() % 4;
        Vector2 pos;
        if (edge == 0) pos = Vector2((float)(rand() % RM.WINDOW_WIDTH), -50.0f);
        else if (edge == 1) pos = Vector2((float)(rand() % RM.WINDOW_WIDTH), RM.WINDOW_HEIGHT + 50.0f);
        else if (edge == 2) pos = Vector2(-50.0f, (float)(rand() % RM.WINDOW_HEIGHT));
        else pos = Vector2(RM.WINDOW_WIDTH + 50.0f, (float)(rand() % RM.WINDOW_HEIGHT));

        float speed = 120.0f + (currentWave * 5.0f);
        SPAWN.SpawnObject(new ArenaEnemy(pos, speed));
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
        GameConfig::pendingMode = 6;
        GameConfig::pendingScore = score;
        SM.SetNextScene("GameOver");
    }
};