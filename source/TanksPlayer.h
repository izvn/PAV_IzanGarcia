#pragma once
#include "Player.h"
#include "InputManager.h"
#include "Spawner.h"
#include "TanksPlayerBullet.h"
#include "AnimatedImageRenderer.h"
#include <cmath>

class TanksPlayer : public Player {
private:
    float speed;
    float shootTimer;

public:
    TanksPlayer()
        : Player("resources/player_tank.png", Vector2(0, 0), Vector2(32, 64)),
        speed(150.0f),
        shootTimer(0)
    {
        transform->position = Vector2(RM.WINDOW_WIDTH / 2, RM.WINDOW_HEIGHT / 2);
        transform->scale = Vector2(1.5f, 1.5f);

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        renderer = new AnimatedImageRenderer(
            transform,
            "resources/player_tank.png",
            Vector2(0, 0),
            64,
            128,
            2,
            0.5f,
            true
        );
    }

    void Update() override {
        HandleInput();
        WrapScreen();
        Player::Update();
        shootTimer += TIME.GetDeltaTime();
    }

    void HandleInput() override {
        Vector2 vel(0, 0);
        if (Input.GetEvent(SDLK_w, HOLD)) vel.y = -1;
        if (Input.GetEvent(SDLK_s, HOLD)) vel.y = 1;
        if (Input.GetEvent(SDLK_a, HOLD)) vel.x = -1;
        if (Input.GetEvent(SDLK_d, HOLD)) vel.x = 1;

        if (vel.x != 0 || vel.y != 0) {
            vel.Normalize();
            physics->SetVelocity(vel * speed);

            float angleRad = atan2(vel.y, vel.x);
            float angleDeg = angleRad * (180.0f / M_PI);
            angleDeg = round(angleDeg / 45.0f) * 45.0f;
            transform->rotation = angleDeg;
        }
        else {
            physics->SetVelocity(Vector2(0, 0));
        }

        if (Input.GetLeftClick() && CanShoot() && shootTimer >= 1.0f) {
            Shoot();
            shootTimer = 0;
        }
    }

    void Shoot() override {
        AM.PlayClip("tank_player_shoot", 0);
        ResetShoot();

        int mouseX = Input.GetMouseX();
        int mouseY = Input.GetMouseY();
        Vector2 dir(mouseX - transform->position.x, mouseY - transform->position.y);
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