#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"

class FroggerEnemy : public Enemy {
private:
    float speed;
    float direction;

public:
    FroggerEnemy(Vector2 pos, float spd, float dir)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32)),
        speed(spd), direction(dir)
    {
        transform->position = pos;
        transform->scale = Vector2(2.0f, 1.5f);
        physics->SetVelocity(Vector2(speed * direction, 0));

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
        Enemy::Update();
        WrapScreen();
    }

    void WrapScreen() {
        float halfWidth = (transform->size.x * transform->scale.x) / 2.0f;
        if (direction > 0 && transform->position.x - halfWidth > RM.WINDOW_WIDTH) {
            transform->position.x = -halfWidth;
        }
        else if (direction < 0 && transform->position.x + halfWidth < 0) {
            transform->position.x = RM.WINDOW_WIDTH + halfWidth;
        }
    }
};