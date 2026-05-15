#pragma once
#include "Bullet.h"

class VortexBullet : public Bullet {
public:
    VortexBullet(Vector2 startPos, Vector2 dir)
        : Bullet("resources/bullet.png", startPos, Vector2(16, 16), dir, 600.0f) {
    }

    void OnCollisionEnter(Object* other) override {
        if (Enemy* e = dynamic_cast<Enemy*>(other)) {
            AM.PlayClip("tank_enemy_death", 0);
            e->Destroy();
            Destroy();
        }
    }
};