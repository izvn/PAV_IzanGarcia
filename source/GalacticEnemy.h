#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"
#include "ExplosionEffect.h"
#include "Spawner.h"
#include <cmath>

class GalacticEnemy : public Enemy {
private:
    float speed;
    float amplitude;
    float startX;
    float timeAlive;

public:
    GalacticEnemy(Vector2 pos, float spd, float amp)
        : Enemy(GameConfig::GetEnemySkin("enemy"), pos, Vector2(32, 32)),
        speed(spd), amplitude(amp), startX(pos.x), timeAlive(0.0f)
    {
        transform->scale = Vector2(1.5f, 1.5f);
        transform->rotation = 180.0f;

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }
        renderer = new AnimatedImageRenderer(transform, GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), 32, 32, 2, 0.2f, true);
    }

    void Update() override {
        timeAlive += TIME.GetDeltaTime();

        float newX = startX;
        if (amplitude > 0.0f) {
            newX = startX + std::sin(timeAlive * 3.0f) * amplitude;
        }

        physics->SetVelocity(Vector2(0, speed));
        transform->position.x = newX;

        Enemy::Update();

        if (transform->position.y > RM.WINDOW_HEIGHT + 100.0f) {
            ImageObject::Destroy();
        }
    }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        Enemy::Destroy();
    }
};