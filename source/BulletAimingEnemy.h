#pragma once
#include "Bullet.h"
#include "Player.h"


class BulletAimingEnemy : public Bullet {
public:
    BulletAimingEnemy(Vector2 startPos, Vector2 playerPos)
        : Bullet("resources/bullet_enemy.png",
            startPos,
            Vector2(16, 16),
            Vector2(), 
            300.0f)
    {
        Vector2 dir = playerPos - startPos;
        dir.Normalize();
        physics->SetVelocity(dir * speed);
    }

    void OnCollisionEnter(Object* other) override {
        if (Player* p = dynamic_cast<Player*>(other)) {
            p->LoseLife();
            Destroy();
        }
    }
};
