#pragma once
#include "Bullet.h"
#include "Enemy.h"
#include "AudioManager.h"

class GalacticPlayerBullet : public Bullet {
public:
    GalacticPlayerBullet(Vector2 startPos)
        : Bullet("resources/bullet.png", startPos, Vector2(16, 16), Vector2(0, -1), 800.0f) {
    }

    void OnCollisionEnter(Object* other) override {
        if (Enemy* e = dynamic_cast<Enemy*>(other)) {
            AM.PlayClip("tank_enemy_death", 0);
            e->Destroy();
            Destroy();
        }
    }
};
