#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"
#include "ExplosionEffect.h"
#include "Spawner.h"
#include <cmath>

class ScrollerEnemy : public Enemy {
private:
    float speed;
    float verticalMovement;
    float sineTimer;

public:
    ScrollerEnemy(Vector2 pos, float spd, float verticalRange = 0.0f)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32)),
        speed(spd), verticalMovement(verticalRange), sineTimer(0)
    {
        transform->position = pos;
        transform->scale = Vector2(1.5f, 1.5f);
        transform->rotation = -90.0f;

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

        sineTimer += TIME.GetDeltaTime();
        float velY = sin(sineTimer * 3.0f) * verticalMovement;

        physics->SetVelocity(Vector2(-speed, velY));

        if (transform->position.x < -50.0f) {
            DestroySilently();
        }
    }

    void DestroySilently() {
        ImageObject::Destroy();
    }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        Enemy::Destroy();
    }
};