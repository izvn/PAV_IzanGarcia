#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"

class PacmanGhost : public Enemy {
public:
    Vector2 currentDir;
    float speed;
    Vector2 spawnPos;

    PacmanGhost(Vector2 pos)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32)), speed(170.0f), spawnPos(pos)
    {
        transform->position = pos;
        transform->size = Vector2(24, 24);
        transform->scale = Vector2(1.0f, 1.0f);
        currentDir = Vector2(1, 0);

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }
        renderer = new AnimatedImageRenderer(
            transform, GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), 32, 32, 2, 0.2f, true
        );
    }

    void Update() override {
        Enemy::Update();
    }
};