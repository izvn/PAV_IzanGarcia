#pragma once
#include "Bullet.h"
#include "Player.h"

class EnemyBullet : public Bullet {
public:
    
    EnemyBullet(Vector2 startPos, Vector2 dir = Vector2(0, 1))
        : Bullet("resources/bullet_enemy.png",
            startPos,
            Vector2(16, 16),
            dir,
            300.0f)
    {

    }

    void OnCollisionEnter(Object* other) override {

        if (Player* p = dynamic_cast<Player*>(other)) {
            p->LoseLife();
            Destroy();
        }
    }
};
