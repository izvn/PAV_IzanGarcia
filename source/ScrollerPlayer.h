#pragma once
#include "Player.h"
#include "InputManager.h"
#include "AnimatedImageRenderer.h"
#include "ScrollerPlayerBullet.h"
#include "Spawner.h"
#include "RenderManager.h"
#include <cmath>

class ScrollerPlayer : public Player {
private:
    float speed;

public:
    ScrollerPlayer()
        : Player("resources/player.png", Vector2(0, 0), Vector2(32, 32)),
        speed(350.0f)
    {
        transform->position = Vector2(100.0f, RM.WINDOW_HEIGHT / 2.0f);
        transform->scale = Vector2(1.5f, 1.5f);
        transform->rotation = 90.0f;
        shootCooldown = 0.2f;

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
            0.15f,
            true
        );
    }

    void Update() override {
        HandleInput();
        ConfineToScreen();
        Player::Update();
    }

    void HandleInput() override {
        Vector2 vel(0, 0);

        if (Input.GetEvent(SDLK_w, HOLD) || Input.GetEvent(SDLK_w, DOWN)) vel.y = -speed;
        if (Input.GetEvent(SDLK_s, HOLD) || Input.GetEvent(SDLK_s, DOWN)) vel.y = speed;
        if (Input.GetEvent(SDLK_a, HOLD) || Input.GetEvent(SDLK_a, DOWN)) vel.x = -speed;
        if (Input.GetEvent(SDLK_d, HOLD) || Input.GetEvent(SDLK_d, DOWN)) vel.x = speed;

        physics->SetVelocity(vel);

        if ((Input.GetEvent(SDLK_SPACE, DOWN) || Input.GetLeftClick()) && CanShoot()) {
            Shoot();
        }
    }

    void ConfineToScreen() {
        float halfWidth = (transform->size.x * transform->scale.x) / 2.0f;
        float halfHeight = (transform->size.y * transform->scale.y) / 2.0f;

        if (transform->position.x - halfWidth < 0) transform->position.x = halfWidth;
        if (transform->position.x + halfWidth > RM.WINDOW_WIDTH / 2.0f) transform->position.x = RM.WINDOW_WIDTH / 2.0f - halfWidth;
        if (transform->position.y - halfHeight < 0) transform->position.y = halfHeight;
        if (transform->position.y + halfHeight > RM.WINDOW_HEIGHT) transform->position.y = RM.WINDOW_HEIGHT - halfHeight;
    }

    void Shoot() override {
        AM.PlayClip("space_invaders_player_shoot", 0);
        ResetShoot();

        Vector2 bulletPos = transform->position;
        bulletPos.x += (transform->size.y / 2) * transform->scale.y;

        SPAWN.SpawnObject(new ScrollerPlayerBullet(bulletPos, Vector2(1, 0)));
    }

    void OnCollisionEnter(Object* other) override {
        if (Enemy* e = dynamic_cast<Enemy*>(other)) {
            LoseLife();
            AM.PlayClip("splat_player_hit", 0);
            e->Destroy();
        }
    }
};