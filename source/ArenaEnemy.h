#pragma once
#include "Enemy.h"
#include "ArenaPlayer.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "ExplosionEffect.h"
#include "SceneManager.h"
#include <cmath>

class ArenaEnemy : public Enemy {
private:
    float speed;
public:
    ArenaEnemy(Vector2 pos, float spd)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32)),
        speed(spd)
    {
        transform->position = pos;
        transform->scale = Vector2(1.5f, 1.5f);

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
        ArenaPlayer* player = FindPlayer();
        if (player) {
            float dx = player->GetTransform()->position.x - transform->position.x;
            float dy = player->GetTransform()->position.y - transform->position.y;
            Vector2 dir(dx, dy);
            dir.Normalize();
            physics->SetVelocity(dir * speed);
        }
    }

    ArenaPlayer* FindPlayer() {
        Scene* sc = SM.GetCurrentScene();
        for (auto o : sc->GetObjects()) {
            if (ArenaPlayer* p = dynamic_cast<ArenaPlayer*>(o)) {
                return p;
            }
        }
        return nullptr;
    }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        Enemy::Destroy();
    }
};