#pragma once
#include "Scene.h"
#include "TanksPlayer.h"
#include "BasicEnemyTanks.h"
#include "SeekerEnemy.h"
#include "ShootingEnemy.h"
#include "AimingEnemy.h"
#include "ExplodingEnemy.h"
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

class TanksGameplay : public Scene {
private:
    TanksPlayer* player;
    bool gameOver;

    int totalWaves;
    int activeEnemies;

    SDL_Texture* backgroundTexture;
    int score;
    int highScore;
    TextObject* scoreText;
    TextObject* livesText;
    TextObject* highScoreText;

    bool paused;

    struct EnemyInfo {
        std::string type;
        int count;
        float speed;
        float health;
        float shootInterval;
    };
    std::vector<std::vector<EnemyInfo>> wavesData;

public:
    TanksGameplay()
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
        AM.PlaySong("tank_music");

        srand((unsigned)time(NULL));

        player = new TanksPlayer();
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

        LoadWavesFromXML("resources/tanks_waves.xml");
        if (!wavesData.empty()) {
            totalWaves = (int)wavesData.size();
            SpawnRandomWave();
        }
    }

    void OnExit() override {
        if (SM.GetNextSceneName() == "TanksPause")
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
            SM.SetNextScene("TanksPause");
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
        wavesData.clear();
        try {
            rapidxml::file<> xmlFile(filePath.c_str());
            rapidxml::xml_document<> doc;
            doc.parse<0>(xmlFile.data());

            rapidxml::xml_node<>* root = doc.first_node("waves");
            if (!root) {
                return;
            }

            for (auto* waveNode = root->first_node("wave"); waveNode; waveNode = waveNode->next_sibling("wave")) {
                std::vector<EnemyInfo> waveEnemies;
                for (auto* enemyNode = waveNode->first_node("enemy"); enemyNode; enemyNode = enemyNode->next_sibling("enemy")) {
                    EnemyInfo ei;
                    ei.type = enemyNode->first_attribute("type")->value();
                    ei.count = std::stoi(enemyNode->first_attribute("count")->value());
                    ei.speed = std::stof(enemyNode->first_attribute("speed")->value());
                    ei.health = std::stof(enemyNode->first_attribute("health")->value());
                    if (enemyNode->first_attribute("shootInterval")) {
                        ei.shootInterval = std::stof(enemyNode->first_attribute("shootInterval")->value());
                    }
                    else {
                        ei.shootInterval = 2.0f;
                    }
                    waveEnemies.push_back(ei);
                }
                wavesData.push_back(waveEnemies);
            }
        }
        catch (std::exception& e) {
        }
    }

    void SpawnRandomWave() {
        if (wavesData.empty()) return;

        int randomIndex = rand() % wavesData.size();

        for (auto& ei : wavesData[randomIndex]) {
            for (int i = 0; i < ei.count; i++) {
                SpawnEnemy(ei);
                activeEnemies++;
            }
        }
        AM.PlayClip("tank_end_wave", 0);
    }

    void SpawnEnemy(const EnemyInfo& ei) {
        Vector2 pos = GetRandomPositionInsideGameArea();

        Enemy* newEnemy = nullptr;
        if (ei.type == "BasicEnemyTanks") {
            newEnemy = new BasicEnemyTanks(pos);
        }
        else if (ei.type == "SeekerEnemy") {
            newEnemy = new SeekerEnemy(pos);
        }
        else if (ei.type == "ShootingEnemy") {
            newEnemy = new ShootingEnemy(pos);
        }
        else if (ei.type == "AimingEnemy") {
            newEnemy = new AimingEnemy(pos);
        }
        else if (ei.type == "ExplodingEnemy") {
            newEnemy = new ExplodingEnemy(pos);
        }
        else {
            newEnemy = new BasicEnemyTanks(pos);
        }

        SPAWN.SpawnObject(newEnemy);
    }

    Vector2 GetRandomPositionInsideGameArea() {
        int padding = 64;
        return Vector2(
            padding + rand() % (RM.WINDOW_WIDTH - 2 * padding),
            padding + rand() % (RM.WINDOW_HEIGHT - 2 * padding)
        );
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
        GameConfig::pendingMode = 1;
        GameConfig::pendingScore = score;
        SM.SetNextScene("GameOver");
    }
};