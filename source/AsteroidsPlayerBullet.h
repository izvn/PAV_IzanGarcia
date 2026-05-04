#pragma once
#include "Bullet.h"
#include "Enemy.h"
#include "AudioManager.h"
#include "RenderManager.h"

class AsteroidsPlayerBullet : public Bullet {
public:
    AsteroidsPlayerBullet(Vector2 startPos, Vector2 direction)
        : Bullet("resources/bullet.png",
            startPos,
            Vector2(16, 16),
            direction,
            600.0f)
    {
    }

    void Update() override {
        Bullet::Update();
        WrapScreen();
    }

    void WrapScreen() {
        if (transform->position.x < 0) transform->position.x = (float)RM.WINDOW_WIDTH;
        if (transform->position.x > RM.WINDOW_WIDTH) transform->position.x = 0;
        if (transform->position.y < 0) transform->position.y = (float)RM.WINDOW_HEIGHT;
        if (transform->position.y > RM.WINDOW_HEIGHT) transform->position.y = 0;
    }

    void OnCollisionEnter(Object* other) override {
        if (Enemy* e = dynamic_cast<Enemy*>(other)) {
            AM.PlayClip("tank_enemy_death", 0);
            e->Destroy();
            Destroy();
        }
    }
};