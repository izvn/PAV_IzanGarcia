#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"

class FlappyObstacle : public Enemy {
private:
    float speed;
    bool isScoreTrigger;
    bool passed;

public:
    FlappyObstacle(Vector2 pos, float spd, bool scoreTrigger, float rot)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32)),
        speed(spd), isScoreTrigger(scoreTrigger), passed(false)
    {
        transform->position = pos;
        transform->scale = Vector2(1.5f, 1.5f);
        transform->rotation = rot;

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        renderer = new AnimatedImageRenderer(
            transform,
            GameConfig::GetEnemySkin("enemy"),
            Vector2(0, 0),
            32,
            32,
            2,
            0.5f,
            true
        );
    }

    void Update() override {
        physics->SetVelocity(Vector2(-speed, 0));
        Enemy::Update();

        if (transform->position.x < -100.0f) {
            ImageObject::Destroy();
        }
    }

    bool CheckPassed(float playerX) {
        if (isScoreTrigger && !passed && transform->position.x < playerX) {
            passed = true;
            return true;
        }
        return false;
    }

    void Destroy() override {
        ImageObject::Destroy();
    }
};