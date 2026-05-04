#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "ExplosionEffect.h"
#include "Spawner.h"
#include "RenderManager.h"
#include <cstdlib>
#include <cmath>

class AsteroidEnemy : public Enemy {
public:
    int sizeLevel;

    AsteroidEnemy(Vector2 pos, int level, Vector2 velocity)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32)),
        sizeLevel(level)
    {
        transform->position = pos;
        transform->scale = Vector2(0.5f * level + 0.5f, 0.5f * level + 0.5f);

        physics->SetVelocity(velocity);

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
        if (transform->position.x < 0) transform->position.x = (float)RM.WINDOW_WIDTH;
        if (transform->position.x > RM.WINDOW_WIDTH) transform->position.x = 0;
        if (transform->position.y < 0) transform->position.y = (float)RM.WINDOW_HEIGHT;
        if (transform->position.y > RM.WINDOW_HEIGHT) transform->position.y = 0;
    }

    void Destroy() override {
        if (sizeLevel > 1) {
            for (int i = 0; i < 2; ++i) {
                float angle = (rand() % 360) * (M_PI / 180.0f);
                float speed = 80.0f + (rand() % 60);
                Vector2 newVel(cos(angle) * speed, sin(angle) * speed);
                SPAWN.SpawnObject(new AsteroidEnemy(transform->position, sizeLevel - 1, newVel));
            }
        }
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        Enemy::Destroy();
    }
};