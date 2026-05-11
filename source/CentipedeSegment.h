#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"

class CentipedeSegment : public Enemy {
public:
    float speed;
    int direction;

    CentipedeSegment(Vector2 pos, int dir, float spd)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32)),
        speed(spd), direction(dir)
    {
        transform->position = pos;
        transform->scale = Vector2(1.0f, 1.0f);

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
            0.2f,
            true
        );
    }

    void Update() override {
        physics->SetVelocity(Vector2(speed * direction, 0));
        Enemy::Update();

        float halfWidth = (transform->size.x * transform->scale.x) / 2.0f;
        if ((direction > 0 && transform->position.x + halfWidth >= RM.WINDOW_WIDTH) ||
            (direction < 0 && transform->position.x - halfWidth <= 0)) {
            DropAndReverse();
        }
    }

    void DropAndReverse() {
        transform->position.y += 32.0f;
        direction *= -1;

        if (transform->position.y > RM.WINDOW_HEIGHT + 32.0f) {
            transform->position.y = 32.0f;
        }

        float halfWidth = (transform->size.x * transform->scale.x) / 2.0f;
        if (direction > 0) transform->position.x = halfWidth + 2.0f;
        else transform->position.x = RM.WINDOW_WIDTH - halfWidth - 2.0f;
    }
};