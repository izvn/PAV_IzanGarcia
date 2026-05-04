#pragma once
#include "Player.h"
#include "InputManager.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"

class BreakoutPlayer : public Player {
private:
    float speed;

public:
    BreakoutPlayer()
        : Player("resources/player.png", Vector2(0, 0), Vector2(32, 32)),
        speed(500.0f)
    {
        transform->position = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT - 80.0f);
        transform->scale = Vector2(3.0f, 1.0f);

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        renderer = new AnimatedImageRenderer(
            transform,
            "resources/player.png",
            Vector2(0, 0),
            32,
            32,
            2,
            0.5f,
            true
        );
    }

    void Update() override {
        HandleInput();
        Player::Update();
        ConfineToScreen();
    }

    void HandleInput() override {
        Vector2 vel(0, 0);
        if (Input.GetEvent(SDLK_a, HOLD) || Input.GetEvent(SDLK_a, DOWN)) vel.x = -speed;
        if (Input.GetEvent(SDLK_d, HOLD) || Input.GetEvent(SDLK_d, DOWN)) vel.x = speed;

        physics->SetVelocity(vel);
    }

    void ConfineToScreen() {
        float halfWidth = (transform->size.x * transform->scale.x) / 2.0f;
        if (transform->position.x - halfWidth < 0) {
            transform->position.x = halfWidth;
        }
        if (transform->position.x + halfWidth > RM.WINDOW_WIDTH) {
            transform->position.x = RM.WINDOW_WIDTH - halfWidth;
        }
    }

    void Shoot() override {}
};