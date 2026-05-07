#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"
#include "ExplosionEffect.h"
#include "Spawner.h"
#include <cmath>

class AsteroidsEnemy : public Enemy {
public:
    int sizeLevel;
private:
    Vector2 velocity;

public:
    AsteroidsEnemy(Vector2 pos, int size, Vector2 vel)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32)),
        sizeLevel(size), velocity(vel)
    {
        transform->position = pos;

        float scaleMultiplier = (float)sizeLevel * 0.8f;
        transform->scale = Vector2(scaleMultiplier, scaleMultiplier);

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
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));

        if (sizeLevel > 1) {
            for (int i = 0; i < 2; i++) {
                float angle = (rand() % 360) * (M_PI / 180.0f);
                Vector2 currentVel = physics->GetVelocity();
                float speed = std::sqrt(currentVel.x * currentVel.x + currentVel.y * currentVel.y) * 1.3f;
                Vector2 newVel(cos(angle) * speed, sin(angle) * speed);
                SPAWN.SpawnObject(new AsteroidsEnemy(transform->position, sizeLevel - 1, newVel));
            }
        }

        Enemy::Destroy();
    }
};