#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"
#include "ExplosionEffect.h"
#include "Spawner.h"
#include <cmath>
#include <cstdlib>

class AsteroidsEnemy : public Enemy {
public:
    int sizeLevel;

    AsteroidsEnemy(Vector2 pos, Vector2 vel, int sizeLevel)
        : Enemy(GameConfig::GetEnemySkin("enemy"), pos, Vector2(32, 32)), sizeLevel(sizeLevel)
    {
        if (sizeLevel == 3) transform->scale = Vector2(2.0f, 2.0f);
        else if (sizeLevel == 2) transform->scale = Vector2(1.2f, 1.2f);
        else transform->scale = Vector2(0.6f, 0.6f);

        physics->SetVelocity(vel);

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }
        renderer = new AnimatedImageRenderer(transform, GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), 32, 32, 2, 0.2f, true);
    }

    void Update() override {
        Enemy::Update();
        if (transform->position.x < -50) transform->position.x = RM.WINDOW_WIDTH + 50;
        if (transform->position.x > RM.WINDOW_WIDTH + 50) transform->position.x = -50;
        if (transform->position.y < -50) transform->position.y = RM.WINDOW_HEIGHT + 50;
        if (transform->position.y > RM.WINDOW_HEIGHT + 50) transform->position.y = -50;
    }

    void Destroy() override {
        ExplosionEffect* exp = new ExplosionEffect(transform->position);
        exp->GetTransform()->scale = transform->scale;
        SPAWN.SpawnObject(exp);

        if (sizeLevel > 1) {
            float speedMag = std::sqrt(physics->GetVelocity().x * physics->GetVelocity().x + physics->GetVelocity().y * physics->GetVelocity().y);
            float newSpeed = speedMag * 1.3f;
            for (int i = 0; i < 2; i++) {
                float angle = (rand() % 360) * (3.14159f / 180.0f);
                Vector2 newVel(std::cos(angle) * newSpeed, std::sin(angle) * newSpeed);
                SPAWN.SpawnObject(new AsteroidsEnemy(transform->position, newVel, sizeLevel - 1));
            }
        }
        Enemy::Destroy();
    }
};