#pragma once
#include "Player.h"
#include "InputManager.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"

class FroggerPlayer : public Player {
private:
    float stepSize;
    Vector2 startPos;

public:
    FroggerPlayer()
        : Player("resources/player.png", Vector2(0, 0), Vector2(32, 32)),
        stepSize(64.0f)
    {
        startPos = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT - 48.0f);
        transform->position = startPos;
        transform->scale = Vector2(1.5f, 1.5f);

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
        ConfineToScreen();
        Player::Update();
    }

    void HandleInput() override {
        if (Input.GetEvent(SDLK_w, DOWN)) transform->position.y -= stepSize;
        if (Input.GetEvent(SDLK_s, DOWN)) transform->position.y += stepSize;
        if (Input.GetEvent(SDLK_a, DOWN)) transform->position.x -= stepSize;
        if (Input.GetEvent(SDLK_d, DOWN)) transform->position.x += stepSize;

        if (Input.GetEvent(SDLK_w, DOWN) || Input.GetEvent(SDLK_s, DOWN) || Input.GetEvent(SDLK_a, DOWN) || Input.GetEvent(SDLK_d, DOWN)) {
            AM.PlayClip("button_hover", 0);
        }
    }

    void ConfineToScreen() {
        float halfWidth = (transform->size.x * transform->scale.x) / 2.0f;
        float halfHeight = (transform->size.y * transform->scale.y) / 2.0f;

        if (transform->position.x - halfWidth < 0) transform->position.x = halfWidth;
        if (transform->position.x + halfWidth > RM.WINDOW_WIDTH) transform->position.x = RM.WINDOW_WIDTH - halfWidth;
        if (transform->position.y + halfHeight > RM.WINDOW_HEIGHT) transform->position.y = RM.WINDOW_HEIGHT - halfHeight;
    }

    void ResetPosition() {
        transform->position = startPos;
    }

    void Shoot() override {}
};