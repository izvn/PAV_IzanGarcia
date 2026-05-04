#pragma once
#include "Player.h"
#include "InputManager.h"
#include "AsteroidsPlayerBullet.h"
#include "Spawner.h"
#include "AnimatedImageRenderer.h"
#include "RenderManager.h"
#include <cmath>

class AsteroidsPlayer : public Player {
private:
    float thrustPower;
    float rotationSpeed;

public:
    AsteroidsPlayer()
        : Player("resources/player.png", Vector2(0, 0), Vector2(32, 32)),
        thrustPower(500.0f), rotationSpeed(250.0f)
    {
        transform->position = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f);
        transform->scale = Vector2(1.5f, 1.5f);
        transform->rotation = 0.0f;
        shootCooldown = 0.3f;

        physics->SetLinearDrag(1.2f);

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
        WrapScreen();
        Player::Update();
    }

    void HandleInput() override {
        if (Input.GetEvent(SDLK_a, HOLD) || Input.GetEvent(SDLK_a, DOWN)) {
            transform->rotation -= rotationSpeed * TIME.GetDeltaTime();
        }
        if (Input.GetEvent(SDLK_d, HOLD) || Input.GetEvent(SDLK_d, DOWN)) {
            transform->rotation += rotationSpeed * TIME.GetDeltaTime();
        }

        if (Input.GetEvent(SDLK_w, HOLD) || Input.GetEvent(SDLK_w, DOWN)) {
            float angleRad = transform->rotation * (M_PI / 180.0f);
            Vector2 forward(sin(angleRad), -cos(angleRad));
            physics->AddForce(forward * thrustPower);
        }

        if ((Input.GetEvent(SDLK_SPACE, DOWN) || Input.GetLeftClick()) && CanShoot()) {
            Shoot();
        }
    }

    void Shoot() override {
        AM.PlayClip("space_invaders_player_shoot", 0);
        ResetShoot();

        float angleRad = transform->rotation * (M_PI / 180.0f);
        Vector2 forward(sin(angleRad), -cos(angleRad));

        Vector2 bulletPos = transform->position;
        bulletPos.x += forward.x * (transform->size.x / 2) * transform->scale.x;
        bulletPos.y += forward.y * (transform->size.y / 2) * transform->scale.y;

        SPAWN.SpawnObject(new AsteroidsPlayerBullet(bulletPos, forward));
    }

    void WrapScreen() {
        if (transform->position.x < 0) transform->position.x = (float)RM.WINDOW_WIDTH;
        if (transform->position.x > RM.WINDOW_WIDTH) transform->position.x = 0;
        if (transform->position.y < 0) transform->position.y = (float)RM.WINDOW_HEIGHT;
        if (transform->position.y > RM.WINDOW_HEIGHT) transform->position.y = 0;
    }

    void OnCollisionEnter(Object* other) override {
        if (Enemy* e = dynamic_cast<Enemy*>(other)) {
            LoseLife();
            e->Destroy();
        }
    }
};