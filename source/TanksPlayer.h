#pragma once
#include "Player.h"
#include "InputManager.h"
#include "Spawner.h"
#include "TanksPlayerBullet.h"
#include "AnimatedImageRenderer.h"
#include "ImageRenderer.h"
#include <cmath>

class TanksPlayer : public Player {
private:
    float speed;
    AnimatedImageRenderer* baseRenderer;
    ImageRenderer* turretRenderer;
    Transform* turretTransform;

public:
    TanksPlayer()
        : Player("resources/tank_base.png", Vector2(0, 0), Vector2(32, 32)),
        speed(150.0f)
    {
        transform->position = Vector2(RM.WINDOW_WIDTH / 2, RM.WINDOW_HEIGHT / 2);
        transform->scale = Vector2(1.5f, 1.5f);
        shootCooldown = 1.0f;

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        baseRenderer = new AnimatedImageRenderer(
            transform,
            "resources/tank_base.png",
            Vector2(0, 0),
            32,
            32,
            2,
            0.5f,
            true
        );

        turretTransform = new Transform();
        turretTransform->position = transform->position;
        turretTransform->scale = transform->scale;
        turretTransform->size = Vector2(32, 32);

        turretRenderer = new ImageRenderer(
            turretTransform,
            "resources/tank_turret.png",
            Vector2(0, 0),
            Vector2(32, 32)
        );
    }

    ~TanksPlayer() override {
        if (baseRenderer) {
            delete baseRenderer;
            baseRenderer = nullptr;
        }
        if (turretRenderer) {
            delete turretRenderer;
            turretRenderer = nullptr;
        }
        if (turretTransform) {
            delete turretTransform;
            turretTransform = nullptr;
        }
    }

    void Update() override {
        HandleInput();
        WrapScreen();
        Player::Update();

        turretTransform->position = transform->position;

        if (baseRenderer) baseRenderer->Update(TIME.GetDeltaTime());
        if (turretRenderer) turretRenderer->Update(TIME.GetDeltaTime());
    }

    void Render() override {
        if (baseRenderer) baseRenderer->Render();
        if (turretRenderer) turretRenderer->Render();
    }

    void HandleInput() override {
        Vector2 vel(0, 0);

        if (Input.GetEvent(SDLK_w, HOLD) || Input.GetEvent(SDLK_w, DOWN)) vel.y -= 1;
        if (Input.GetEvent(SDLK_s, HOLD) || Input.GetEvent(SDLK_s, DOWN)) vel.y += 1;
        if (Input.GetEvent(SDLK_a, HOLD) || Input.GetEvent(SDLK_a, DOWN)) vel.x -= 1;
        if (Input.GetEvent(SDLK_d, HOLD) || Input.GetEvent(SDLK_d, DOWN)) vel.x += 1;

        if (vel.x != 0 || vel.y != 0) {
            vel.Normalize();
            physics->SetVelocity(vel * speed);

            float baseAngleRad = atan2(vel.y, vel.x);
            float baseAngleDeg = baseAngleRad * (180.0f / M_PI);
            baseAngleDeg = round(baseAngleDeg / 45.0f) * 45.0f;
            transform->rotation = baseAngleDeg + 90.0f;
        }
        else {
            physics->SetVelocity(Vector2(0, 0));
        }

        int mx = Input.GetMouseX();
        int my = Input.GetMouseY();
        Vector2 aimDir(mx - transform->position.x, my - transform->position.y);

        if (aimDir.x != 0 || aimDir.y != 0) {
            float turretAngleRad = atan2(aimDir.y, aimDir.x);
            turretTransform->rotation = turretAngleRad * (180.0f / M_PI) + 90.0f;
        }

        if (Input.GetLeftClick() && CanShoot()) {
            Shoot();
        }
    }

    void Shoot() override {
        AM.PlayClip("tank_player_shoot", 0);
        ResetShoot();

        int mx = Input.GetMouseX();
        int my = Input.GetMouseY();
        Vector2 dir(mx - transform->position.x, my - transform->position.y);
        dir.Normalize();

        Vector2 bulletPos = transform->position;
        bulletPos.x += dir.x * (transform->size.x / 2) * transform->scale.x;
        bulletPos.y += dir.y * (transform->size.y / 2) * transform->scale.y;

        SPAWN.SpawnObject(new TanksPlayerBullet(bulletPos, dir));
    }

    void WrapScreen() {
        if (transform->position.x < 0) transform->position.x = (float)RM.WINDOW_WIDTH;
        if (transform->position.x > RM.WINDOW_WIDTH) transform->position.x = 0;
        if (transform->position.y < 0) transform->position.y = (float)RM.WINDOW_HEIGHT;
        if (transform->position.y > RM.WINDOW_HEIGHT) transform->position.y = 0;
    }
};