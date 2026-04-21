#pragma once
#include "Bullet.h"
#include "Enemy.h"
#include "AudioManager.h"

class TanksPlayerBullet : public Bullet {
public:
    TanksPlayerBullet(Vector2 startPos, Vector2 direction)
        : Bullet("resources/bullet.png",  
            startPos,
            Vector2(16, 16),
            direction,
            400.0f)
    {
    }

    void OnCollisionEnter(Object* other) override {
        if (Enemy* e = dynamic_cast<Enemy*>(other)) {
            e->Destroy();
            Destroy();
        }
    }
};
