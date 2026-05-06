#pragma once
#include "Player.h"
#include "InputManager.h"
#include "ImageRenderer.h"
#include "RenderManager.h"
#include <cmath>

class FlappyPlayer : public Player {
private:
    float gravity;
    float jumpForce;
    ImageRenderer* idleRenderer;
    ImageRenderer* thrustRenderer;
    bool hasStarted;

public:
    FlappyPlayer()
        : Player("resources/player.png", Vector2(0, 0), Vector2(32, 32)),
        gravity(1300.0f), jumpForce(-400.0f), hasStarted(false)
    {
        transform->position = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f);
        transform->scale = Vector2(1.5f, 1.5f);
        transform->rotation = 90.0f;

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        idleRenderer = new ImageRenderer(transform, "resources/player.png", Vector2(0, 0), Vector2(32, 32));
        thrustRenderer = new ImageRenderer(transform, "resources/player.png", Vector2(32, 0), Vector2(32, 32));

        renderer = idleRenderer;
        shootCooldown = 0.05f;
    }

    ~FlappyPlayer() override {
        if (renderer == idleRenderer) renderer = nullptr;
        if (renderer == thrustRenderer) renderer = nullptr;
        if (idleRenderer) { delete idleRenderer; idleRenderer = nullptr; }
        if (thrustRenderer) { delete thrustRenderer; thrustRenderer = nullptr; }
    }

    void Update() override {
        if (hasStarted) {
            physics->AddForce(Vector2(0, gravity));
        }
        else {
            physics->SetVelocity(Vector2(0, 0));
            transform->rotation = 90.0f;
        }

        HandleInput();

        if (hasStarted) {
            if (physics->GetVelocity().y < 0) {
                renderer = thrustRenderer;
            }
            else {
                renderer = idleRenderer;
            }

            float velY = physics->GetVelocity().y;
            float targetRotation = 90.0f + (velY * 0.06f);
            if (targetRotation < 45.0f) targetRotation = 45.0f;
            if (targetRotation > 135.0f) targetRotation = 135.0f;
            transform->rotation = targetRotation;
        }
        else {
            renderer = idleRenderer;
        }

        Player::Update();

        if (hasStarted) {
            CheckBounds();
        }
    }

    void HandleInput() override {
        if ((Input.GetEvent(SDLK_SPACE, DOWN) || Input.GetLeftClick()) && CanShoot()) {
            if (!hasStarted) hasStarted = true;
            physics->SetVelocity(Vector2(0, jumpForce));
            AM.PlayClip("button_hover", 0);
            ResetShoot();
        }
    }

    void CheckBounds() {
        if (transform->position.y < 0 || transform->position.y > RM.WINDOW_HEIGHT) {
            LoseLife();
            AM.PlayClip("splat_player_hit", 0);
        }
    }

    void Shoot() override {}

    void OnCollisionEnter(Object* other) override {
        if (Enemy* e = dynamic_cast<Enemy*>(other)) {
            LoseLife();
            AM.PlayClip("splat_player_hit", 0);
        }
    }

    bool HasStarted() const {
        return hasStarted;
    }
};