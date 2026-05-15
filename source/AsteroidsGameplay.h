#pragma once
#include "Scene.h"
#include "AsteroidsPlayer.h"
#include "AsteroidsEnemy.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "GameConfig.h"
#include "TextObject.h"
#include "RankingScene.h"
#include "SceneManager.h"
#include <cstdlib>
#include <vector>
#include <cmath>

class AsteroidsGameplay : public Scene {
private:
    AsteroidsPlayer* player;
    bool gameOver;
    int currentWave;
    int activeEnemies;
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
    AsteroidsGameplay()
        : Scene(), player(nullptr), gameOver(false), currentWave(1), activeEnemies(0),
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
        activeEnemies = 0;
        currentWave = 1;
        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));
        RM.LoadTexture("resources/wavecompleted.png");
        waveCompletedTex = RM.GetTexture("resources/wavecompleted.png");
        AM.StopMusic();
        AM.PlaySong("space_invaders_music");
        srand((unsigned)time(NULL));
        player = new AsteroidsPlayer();
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
        SpawnWave(currentWave);
    }

    void OnExit() override {
        if (SM.GetNextSceneName() == "AsteroidsPause") {
            paused = true;
            return;
        }
        AM.StopMusic();
        Scene::OnExit();
    }

    void Update() override {
        if (!gameOver && Input.GetEvent(SDLK_ESCAPE, DOWN)) {
            SM.SetNextScene("AsteroidsPause");
            return;
        }
        if (showingWaveCompleted) {
            waveCompletedTimer -= TIME.GetDeltaTime();
            if (waveCompletedTimer <= 0) {
                showingWaveCompleted = false;
                SpawnWave(currentWave);
            }
            return;
        }
        UpdateHUD();
        int currentAsteroids = 0;
        for (int i = (int)objects.size() - 1; i >= 0; i--) {
            if (objects[i]->IsPendingDestroy()) {
                if (AsteroidsEnemy* e = dynamic_cast<AsteroidsEnemy*>(objects[i])) {
                    AddScore(100 * (4 - e->sizeLevel));
                }
                delete objects[i];
                objects.erase(objects.begin() + i);
            }
            else if (dynamic_cast<AsteroidsEnemy*>(objects[i])) {
                currentAsteroids++;
            }
        }
        while (SPAWN.GetSpawnedObjectsCount() > 0) {
            Object* spawned = SPAWN.GetSpawnedObject();
            objects.push_back(spawned);
            if (dynamic_cast<AsteroidsEnemy*>(spawned)) currentAsteroids++;
        }
        activeEnemies = currentAsteroids;
        for (Object* o : objects) {
            o->Update();
        }
        CheckCollisions();
        if (player && !player->IsAlive() && !gameOver) {
            EndGame();
        }
        if (!gameOver && activeEnemies <= 0 && player->IsAlive() && !showingWaveCompleted) {
            ClearProjectiles();
            currentWave++;
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

    void SpawnWave(int waveNum) {
        AM.PlayClip("tank_end_wave", 0);
        int numAsteroids = 3 + waveNum;
        for (int i = 0; i < numAsteroids; i++) {
            Vector2 pos;
            bool safePos = false;
            while (!safePos) {
                pos = Vector2((float)(rand() % RM.WINDOW_WIDTH), (float)(rand() % RM.WINDOW_HEIGHT));
                if (player) {
                    float dx = pos.x - player->GetTransform()->position.x;
                    float dy = pos.y - player->GetTransform()->position.y;
                    if (std::sqrt(dx * dx + dy * dy) > 250.0f) safePos = true;
                }
                else safePos = true;
            }
            float angle = (rand() % 360) * (3.14159f / 180.0f);
            float speed = 60.0f + (waveNum * 15.0f);
            Vector2 vel(std::cos(angle) * speed, std::sin(angle) * speed);
            SPAWN.SpawnObject(new AsteroidsEnemy(pos, vel, 3));
        }
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
        if (player) score += player->GetLives() * 10000;
        gameOver = true;
        GameConfig::pendingMode = 3;
        GameConfig::pendingScore = score;
        SM.SetNextScene("GameOver");
    }
};