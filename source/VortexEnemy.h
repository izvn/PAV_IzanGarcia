#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"
#include "ExplosionEffect.h"
#include "Spawner.h"
#include <cmath>

class VortexEnemy : public Enemy {
public:
    VortexEnemy(Vector2 pos, float speed)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32))
    {
        transform->position = pos;
        transform->scale = Vector2(1.5f, 1.5f);

        Vector2 center(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f);
        float dx = center.x - pos.x;
        float dy = center.y - pos.y;
        float length = std::sqrt(dx * dx + dy * dy);

        Vector2 dir(0, 0);
        if (length != 0) {
            dir.x = dx / length;
            dir.y = dy / length;
        }

        physics->SetVelocity(Vector2(dir.x * speed, dir.y * speed));
        transform->rotation = atan2(dy, dx) * (180.0f / M_PI) - 90.0f;

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }
        renderer = new AnimatedImageRenderer(transform, GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), 32, 32, 2, 0.2f, true);
    }

    void Update() override {
        Enemy::Update();
        if (transform->position.x < -100 || transform->position.x > RM.WINDOW_WIDTH + 100 ||
            transform->position.y < -100 || transform->position.y > RM.WINDOW_HEIGHT + 100) {
            ImageObject::Destroy();
        }
    }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        Enemy::Destroy();
    }
};