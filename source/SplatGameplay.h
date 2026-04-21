#pragma once
#include "Scene.h"
#include "SplatPlayer.h"
#include "SplatBasicEnemy.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "GameConfig.h"
#include "TextObject.h"
#include "RankingScene.h"
#include "dep/inc/rapidxml.hpp"
#include "dep/inc/rapidxml_utils.hpp"
#include "SceneManager.h"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

class SplatGameplay : public Scene {
private:
    SplatPlayer* player;
    bool gameOver;

    std::vector<int> waveCounts;
    int totalWaves;
    int activeEnemies;

    SDL_Texture* backgroundTexture;
    int score;
    int highScore;
    TextObject* scoreText;
    TextObject* livesText;
    TextObject* highScoreText;

    bool paused;

public:
    SplatGameplay()
        : Scene(),
        player(nullptr),
        gameOver(false),
        totalWaves(0),
        activeEnemies(0),
        backgroundTexture(nullptr),
        score(0),
        highScore(0),
        scoreText(nullptr),
        livesText(nullptr),
        highScoreText(nullptr),
        paused(false)
    {
    }

    void OnEnter() override {
        if (paused)
        {
            return;
        }

        gameOver = false;
        score = 0;
        activeEnemies = 0;

        backgroundTexture = RM.GetTexture(GameConfig::GetBackgroundPath(GameConfig::GetSelectedBackground()));
        AM.StopMusic();
        AM.PlaySong("splat_music");

        srand((unsigned)time(NULL));

        player = new SplatPlayer();
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

        LoadWavesFromXML("resources/splat_waves.xml");
        if (!waveCounts.empty()) {
            totalWaves = (int)waveCounts.size();
            SpawnRandomWave();
        }
    }

    void OnExit() override {
        if (SM.GetNextSceneName() == "SplatPause")
        {
            paused = true;
            return;
        }
        AM.StopMusic();
        Scene::OnExit();
    }

    void Update() override {
        if (!gameOver && Input.GetEvent(SDLK_ESCAPE, DOWN))
        {
            SM.SetNextScene("SplatPause");
            return;
        }

        UpdateHUD();

        for (int i = (int)objects.size() - 1; i >= 0; i--) {
            if (objects[i]->IsPendingDestroy()) {
                if (Enemy* e = dynamic_cast<Enemy*>(objects[i])) {
                    AddScore(100);
                    activeEnemies--;
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

        if (!gameOver && activeEnemies <= 0 && totalWaves > 0) {
            SpawnRandomWave();
        }
    }

    void Render() override {
        if (backgroundTexture)
            SDL_RenderCopy(RM.GetRenderer(), backgroundTexture, nullptr, nullptr);
        for (Object* o : objects) {
            o->Render();
        }
    }

private:
    void LoadWavesFromXML(const std::string& filePath) {
        waveCounts.clear();
        try {
            rapidxml::file<> xmlFile(filePath.c_str());
            rapidxml::xml_document<> doc;
            doc.parse<0>(xmlFile.data());

            rapidxml::xml_node<>* root = doc.first_node("waves");
            if (!root) {
                return;
            }

            for (auto* waveNode = root->first_node("wave"); waveNode; waveNode = waveNode->next_sibling("wave")) {
                int count = 0;
                if (waveNode->first_attribute("count")) {
                    count = std::stoi(waveNode->first_attribute("count")->value());
                }
                waveCounts.push_back(count);
            }
        }
        catch (std::exception& e) {
        }
    }

    void SpawnRandomWave() {
        if (waveCounts.empty()) return;
        int randomIndex = rand() % waveCounts.size();
        int count = waveCounts[randomIndex];

        for (int i = 0; i < count; i++) {
            Vector2 pos((float)(rand() % RM.WINDOW_WIDTH), (float)(rand() % RM.WINDOW_HEIGHT));
            SPAWN.SpawnObject(new SplatBasicEnemy(pos));
            activeEnemies++;
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
        if (player) {
            score += player->GetLives() * 10000;
        }
        gameOver = true;
        GameConfig::pendingMode = 2;
        GameConfig::pendingScore = score;
        SM.SetNextScene("GameOver");
    }
};