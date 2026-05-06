#pragma once
#include "Bullet.h"
#include "Enemy.h"
#include "AudioManager.h"
#include "RenderManager.h"

class ScrollerPlayerBullet : public Bullet {
public:
    ScrollerPlayerBullet(Vector2 startPos, Vector2 direction)
        : Bullet("resources/bullet.png",
            startPos,
            Vector2(16, 16),
            direction,
            800.0f)
    {
    }

    void Update() override {
        Bullet::Update();
        if (transform->position.x > RM.WINDOW_WIDTH + 50) {
            Destroy();
        }
    }

    void OnCollisionEnter(Object* other) override {
        if (Enemy* e = dynamic_cast<Enemy*>(other)) {
            AM.PlayClip("tank_enemy_death", 0);
            e->Destroy();
            Destroy();
        }
    }
};