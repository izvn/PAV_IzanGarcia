#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "ExplosionEffect.h"
#include "Spawner.h"

class BreakoutBrick : public Enemy {
public:
    BreakoutBrick(Vector2 pos)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32))
    {
        transform->position = pos;
        transform->scale = Vector2(2.0f, 1.0f);

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

    void OnCollisionEnter(Object* other) override {
    }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        Enemy::Destroy();
    }
};