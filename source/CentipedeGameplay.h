#pragma once
#include "Scene.h"
#include "CentipedePlayer.h"
#include "CentipedeSegment.h"
#include "CentipedeMushroom.h"
#include "CentipedePlayerBullet.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "GameConfig.h"
#include "TextObject.h"
#include "RankingScene.h"
#include "SceneManager.h"
#include <cstdlib>
#include <vector>
#include <cmath>

class CentipedeGameplay : public Scene {
private:
    CentipedePlayer* player;
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
    CentipedeGameplay()
        : Scene(), player(nullptr), gameOver(false), currentWave(1), activeEnemies(0),
        backgroundTexture(nullptr), score(0), highScore(0), scoreText(nullptr),
        livesText(nullptr), highScoreText(nullptr), paused(false),
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
        currentWave = 1;
        activeEnemies = 0;
        showingWaveCompleted = false;

        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));
        RM.LoadTexture("resources/wavecompleted.png");
        waveCompletedTex = RM.GetTexture("resources/wavecompleted.png");

        AM.StopMusic();
        AM.PlaySong("menu_music");

        srand((unsigned)time(NULL));

        player = new CentipedePlayer();
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

        SpawnMushrooms();
        SpawnCentipedeWave();
    }

    void OnExit() override {
        if (SM.GetNextSceneName() == "CentipedePause") {
            paused = true;
            return;
        }
        AM.StopMusic();
        Scene::OnExit();
    }

    void Update() override {
        if (!gameOver && Input.GetEvent(SDLK_ESCAPE, DOWN)) {
            SM.SetNextScene("CentipedePause");
            return;
        }

        if (showingWaveCompleted) {
            waveCompletedTimer -= TIME.GetDeltaTime();
            if (waveCompletedTimer <= 0) {
                showingWaveCompleted = false;
                SpawnCentipedeWave();
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
            if (dynamic_cast<CentipedePlayerBullet*>(o)) o->Destroy();
        }
    }

    void SpawnMushrooms() {
        for (int i = 0; i < 40; i++) {
            float x = 32.0f + (rand() % (RM.WINDOW_WIDTH - 64));
            float y = 64.0f + (rand() % (RM.WINDOW_HEIGHT - 300));
            x = std::round(x / 32.0f) * 32.0f;
            y = std::round(y / 32.0f) * 32.0f;
            SPAWN.SpawnObject(new CentipedeMushroom(Vector2(x, y)));
        }
    }

    void SpawnCentipedeWave() {
        int length = 8 + currentWave * 2;
        float startX = 32.0f;
        float startY = 32.0f;
        float speed = 150.0f + (currentWave * 20.0f);

        for (int i = 0; i < length; i++) {
            SPAWN.SpawnObject(new CentipedeSegment(Vector2(startX - i * 32.0f, startY), 1, speed));
            activeEnemies++;
        }
        AM.PlayClip("tank_end_wave", 0);
    }

    void CheckCollisions() {
        for (int i = 0; i < (int)objects.size(); i++) {
            for (int j = i + 1; j < (int)objects.size(); j++) {
                Object* a = objects[i];
                Object* b = objects[j];

                if (a->IsPendingDestroy() || b->IsPendingDestroy()) continue;

                if (a->GetRigidbody()->CheckCollision(b->GetRigidbody())) {
                    CentipedePlayerBullet* bullet = dynamic_cast<CentipedePlayerBullet*>(a) ? dynamic_cast<CentipedePlayerBullet*>(a) : dynamic_cast<CentipedePlayerBullet*>(b);
                    CentipedeSegment* segment = dynamic_cast<CentipedeSegment*>(a) ? dynamic_cast<CentipedeSegment*>(a) : dynamic_cast<CentipedeSegment*>(b);
                    CentipedeMushroom* mushroom = dynamic_cast<CentipedeMushroom*>(a) ? dynamic_cast<CentipedeMushroom*>(a) : dynamic_cast<CentipedeMushroom*>(b);
                    CentipedePlayer* p = dynamic_cast<CentipedePlayer*>(a) ? dynamic_cast<CentipedePlayer*>(a) : dynamic_cast<CentipedePlayer*>(b);

                    if (bullet && segment) {
                        Vector2 pos = segment->GetTransform()->position;
                        bullet->Destroy();
                        segment->Destroy();
                        activeEnemies--;
                        if (activeEnemies > 0) {
                            SPAWN.SpawnObject(new CentipedeMushroom(pos));
                        }
                        AddScore(100);
                        AM.PlayClip("tank_enemy_death", 0);
                    }
                    else if (bullet && mushroom) {
                        bullet->Destroy();
                        mushroom->TakeDamage();
                        AddScore(10);
                    }
                    else if (segment && mushroom) {
                        segment->DropAndReverse();
                    }
                    else if (p) {
                        if (segment) {
                            p->OnCollisionEnter(segment);
                            segment->Destroy();
                            activeEnemies--;
                        }
                        else if (mushroom) {
                            if (mushroom->IsLethal()) {
                                p->OnCollisionEnter(mushroom);
                                mushroom->Destroy();
                            }
                        }
                    }
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
        GameConfig::pendingMode = 10;
        GameConfig::pendingScore = score;
        SM.SetNextScene("GameOver");
    }
};