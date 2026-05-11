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
    float timeAlive;

public:
    GalacticEnemy(Vector2 pos, float spd, float amp)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32)),
        speed(spd), amplitude(amp), timeAlive(0)
    {
        transform->position = pos;
        transform->scale = Vector2(1.5f, 1.5f);
        transform->rotation = 180.0f;

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        renderer = new AnimatedImageRenderer(
            transform, GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), 32, 32, 2, 0.5f, true
        );
    }

    void Update() override {
        Enemy::Update();
        timeAlive += TIME.GetDeltaTime();

        float velX = cos(timeAlive * 3.0f) * amplitude;
        physics->SetVelocity(Vector2(velX, speed));

        if (transform->position.y > RM.WINDOW_HEIGHT + 50.0f) {
            ImageObject::Destroy();
        }
    }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        Enemy::Destroy();
    }
};