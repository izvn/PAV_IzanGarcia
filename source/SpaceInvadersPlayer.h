#pragma once
#include "Player.h"
#include "InputManager.h"
#include "SpaceInvadersBullet.h"
#include "Spawner.h"
#include "AnimatedImageRenderer.h"

class SpaceInvadersPlayer : public Player {
private:
    float speed;
public:
    SpaceInvadersPlayer()
        : Player("resources/player.png", Vector2(0, 0), Vector2(32, 32)),
        speed(200.0f)
    {
        transform->position = Vector2(680, 700);
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
        Player::Update();
    }

    void HandleInput() override {
        Vector2 vel(0, 0);
        bool leftPressed = (Input.GetEvent(SDLK_a, HOLD) || Input.GetEvent(SDLK_a, DOWN));
        bool rightPressed = (Input.GetEvent(SDLK_d, HOLD) || Input.GetEvent(SDLK_d, DOWN));

        if (leftPressed)       vel.x = -speed;
        else if (rightPressed) vel.x = speed;
        else                   vel.x = 0;

        physics->SetVelocity(vel);

        if (Input.GetLeftClick() && CanShoot()) {
            Shoot();
        }
    }

    void Shoot() override {
        AM.PlayClip("space_invaders_player_shoot", 0);
        ResetShoot();

        Vector2 bulletPos = transform->position;
        bulletPos.y -= (transform->size.y / 2) * transform->scale.y;
        SPAWN.SpawnObject(new SpaceInvadersBullet(bulletPos));
    }
};