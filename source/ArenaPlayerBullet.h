#pragma once
#include "Bullet.h"
#include "Enemy.h"
#include "AudioManager.h"

class ArenaPlayerBullet : public Bullet {
public:
    ArenaPlayerBullet(Vector2 startPos, Vector2 direction)
        : Bullet("resources/bullet.png",
            startPos,
            Vector2(16, 16),
            direction,
            800.0f)
    {
    }

    void OnCollisionEnter(Object* other) override {
        if (Enemy* e = dynamic_cast<Enemy*>(other)) {
            e->Destroy();
            Destroy();
        }
    }
};