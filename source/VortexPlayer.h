#pragma once
#include "Player.h"
#include "InputManager.h"
#include "AnimatedImageRenderer.h"
#include "VortexBullet.h"
#include "Spawner.h"
#include "RenderManager.h"
#include <cmath>
#include <SDL.h>

class VortexPlayer : public Player {
public:
    VortexPlayer()
        : Player("resources/player.png", Vector2(0, 0), Vector2(32, 32))
    {
        transform->position = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f);
        transform->scale = Vector2(1.5f, 1.5f);
        shootCooldown = 0.15f;

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }
        renderer = new AnimatedImageRenderer(transform, "resources/player.png", Vector2(0, 0), 32, 32, 2, 0.15f, true);
    }

    void Update() override {
        HandleInput();
        Player::Update();
    }

    void HandleInput() override {
        // Usamos SDL nativo para obtener el ratón y evitar fallos del InputManager
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        Vector2 mousePos((float)mx, (float)my);

        float dx = mousePos.x - transform->position.x;
        float dy = mousePos.y - transform->position.y;
        float angle = atan2(dy, dx) * (180.0f / M_PI);
        transform->rotation = angle + 90.0f;

        if (Input.GetLeftClick() && CanShoot()) {
            Shoot();
        }
    }

    void Shoot() override {
        AM.PlayClip("space_invaders_player_shoot", 0);
        ResetShoot();

        int mx, my;
        SDL_GetMouseState(&mx, &my);
        Vector2 mousePos((float)mx, (float)my);

        float dx = mousePos.x - transform->position.x;
        float dy = mousePos.y - transform->position.y;
        float length = std::sqrt(dx * dx + dy * dy);

        Vector2 dir(0, -1);
        if (length != 0) {
            dir.x = dx / length;
            dir.y = dy / length;
        }

        Vector2 bulletPos;
        bulletPos.x = transform->position.x + dir.x * 20.0f;
        bulletPos.y = transform->position.y + dir.y * 20.0f;

        SPAWN.SpawnObject(new VortexBullet(bulletPos, dir));
    }

    void OnCollisionEnter(Object* other) override {
        if (dynamic_cast<Enemy*>(other)) {
            LoseLife();
            AM.PlayClip("splat_player_hit", 0);
            other->Destroy();
        }
    }
};