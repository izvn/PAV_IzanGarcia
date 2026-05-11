#pragma once
#include "Enemy.h"
#include "ImageRenderer.h"
#include "RenderManager.h"

class CentipedeMushroom : public Enemy {
public:
    int health;
private:
    float lethalTimer;

public:
    CentipedeMushroom(Vector2 pos)
        : Enemy("resources/enemy_splat.png", Vector2(0, 0), Vector2(32, 32)), health(3), lethalTimer(1.0f)
    {
        transform->position = pos;
        transform->scale = Vector2(1.0f, 1.0f);

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        renderer = new ImageRenderer(
            transform,
            "resources/enemy_splat.png",
            Vector2(0, 0),
            Vector2(32, 32)
        );
    }

    void Update() override {
        Enemy::Update();
        if (lethalTimer > 0.0f) {
            lethalTimer -= TIME.GetDeltaTime();
        }
    }

    void TakeDamage() {
        health--;
        if (health <= 0) {
            Destroy();
        }
    }

    bool IsLethal() const {
        return lethalTimer <= 0.0f;
    }
};