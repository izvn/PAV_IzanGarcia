#pragma once
#include "Player.h"
#include "InputManager.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"

class PacmanPlayer : public Player {
public:
    Vector2 desiredDir;
    Vector2 currentDir;
    float speed;
    Vector2 spawnPos;

    PacmanPlayer(Vector2 pos)
        : Player("resources/player.png", Vector2(0, 0), Vector2(32, 32)), speed(200.0f), spawnPos(pos)
    {
        transform->position = pos;
        transform->size = Vector2(24, 24);
        transform->scale = Vector2(1.0f, 1.0f);
        desiredDir = Vector2(0, 0);
        currentDir = Vector2(0, 0);

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }
        renderer = new AnimatedImageRenderer(
            transform, "resources/player.png", Vector2(0, 0), 32, 32, 2, 0.15f, true
        );
    }

    void Update() override {
        HandleInput();
        Player::Update();
    }

    void HandleInput() override {
        if (Input.GetEvent(SDLK_w, DOWN) || Input.GetEvent(SDLK_UP, DOWN)) desiredDir = Vector2(0, -1);
        if (Input.GetEvent(SDLK_s, DOWN) || Input.GetEvent(SDLK_DOWN, DOWN)) desiredDir = Vector2(0, 1);
        if (Input.GetEvent(SDLK_a, DOWN) || Input.GetEvent(SDLK_LEFT, DOWN)) desiredDir = Vector2(-1, 0);
        if (Input.GetEvent(SDLK_d, DOWN) || Input.GetEvent(SDLK_RIGHT, DOWN)) desiredDir = Vector2(1, 0);
    }

    void Shoot() override {}
};