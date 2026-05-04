#pragma once
#include "ImageObject.h"
#include "BreakoutPlayer.h"
#include "BreakoutBrick.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include <cmath>

class BreakoutBall : public ImageObject {
private:
    float speed;
    Vector2 direction;
    bool attached;

public:
    BreakoutBall()
        : ImageObject("resources/bullet.png", Vector2(0, 0), Vector2(16, 16)),
        speed(450.0f), direction(1.0f, -1.0f), attached(true)
    {
        transform->scale = Vector2(1.5f, 1.5f);
        direction.Normalize();
    }

    void Update() override {
        ImageObject::Update();

        if (attached) {
            BreakoutPlayer* player = FindPlayer();
            if (player) {
                transform->position.x = player->GetTransform()->position.x;
                transform->position.y = player->GetTransform()->position.y - 35.0f;
            }

            if (Input.GetEvent(SDLK_SPACE, DOWN) || Input.GetLeftClick()) {
                attached = false;
                physics->SetVelocity(direction * speed);
            }
        }
        else {
            HandleScreenCollisions();
            physics->SetVelocity(direction * speed);
        }
    }

    void HandleScreenCollisions() {
        if (transform->position.x < 0) {
            transform->position.x = 0;
            direction.x *= -1.0f;
            AM.PlayClip("button_hover", 0);
        }
        else if (transform->position.x > RM.WINDOW_WIDTH) {
            transform->position.x = RM.WINDOW_WIDTH;
            direction.x *= -1.0f;
            AM.PlayClip("button_hover", 0);
        }

        if (transform->position.y < 0) {
            transform->position.y = 0;
            direction.y *= -1.0f;
            AM.PlayClip("button_hover", 0);
        }
        else if (transform->position.y > RM.WINDOW_HEIGHT) {
            BreakoutPlayer* player = FindPlayer();
            if (player) {
                player->LoseLife();
                AM.PlayClip("splat_player_hit", 0);
            }
            ResetBall();
        }
    }

    void ResetBall() {
        attached = true;
        direction = Vector2(1.0f, -1.0f);
        direction.Normalize();
        physics->SetVelocity(Vector2(0, 0));
    }

    void OnCollisionEnter(Object* other) override {
        if (attached) return;

        if (BreakoutPlayer* paddle = dynamic_cast<BreakoutPlayer*>(other)) {
            float hitPoint = transform->position.x - paddle->GetTransform()->position.x;
            float normalizedHit = hitPoint / ((paddle->GetTransform()->size.x * paddle->GetTransform()->scale.x) / 2.0f);

            direction.x = normalizedHit * 1.5f;
            direction.y = -1.0f;
            direction.Normalize();

            transform->position.y = paddle->GetTransform()->position.y - (paddle->GetTransform()->size.y * paddle->GetTransform()->scale.y) / 2.0f - 15.0f;
            AM.PlayClip("button_hover", 0);
        }
        else if (BreakoutBrick* brick = dynamic_cast<BreakoutBrick*>(other)) {
            direction.y *= -1.0f;
            AM.PlayClip("space_invaders_enemy_death", 0);
            brick->Destroy();
        }
    }

    BreakoutPlayer* FindPlayer() {
        Scene* sc = SM.GetCurrentScene();
        for (Object* o : sc->GetObjects()) {
            if (BreakoutPlayer* p = dynamic_cast<BreakoutPlayer*>(o)) {
                return p;
            }
        }
        return nullptr;
    }
};