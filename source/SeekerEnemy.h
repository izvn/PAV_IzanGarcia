#pragma once
#include "Enemy.h"
#include "Player.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include <cmath>

class SeekerEnemy : public Enemy {
private:
    std::string pattern;
    float patternTime;
    float patternTimer;
    int currentStep;
    float detectionRange;

public:
    SeekerEnemy(Vector2 pos)
        : Enemy(GameConfig::GetEnemySkin("enemy_seeker"), Vector2(0, 0), Vector2(32, 32)),
        pattern("135"),
        patternTime(0.8f),
        patternTimer(0),
        currentStep(0),
        detectionRange(500000.0f)
    {
        transform->position = pos;
        transform->scale = Vector2(1.5f, 1.5f);

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        renderer = new AnimatedImageRenderer(
            transform,
            GameConfig::GetEnemySkin("enemy_seeker"),
            Vector2(0, 0),
            32,
            32,
            2,
            0.5f,
            true
        );
    }

    void Update() override {
        Enemy::Update();
        Player* player = FindPlayer();
        if (player) {
            float dx = player->GetTransform()->position.x - transform->position.x;
            float dy = player->GetTransform()->position.y - transform->position.y;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist < detectionRange) {
                Vector2 dir(dx, dy);
                dir.Normalize();
                physics->SetVelocity(dir * 120.0f);
            }
            else {
                patternTimer += TIME.GetDeltaTime();
                if (patternTimer >= patternTime) {
                    patternTimer = 0;
                    currentStep = (currentStep + 1) % pattern.size();
                    MoveAccordingToPattern(pattern[currentStep]);
                }
            }
        }
        WrapScreen();
    }

    Player* FindPlayer() {
        Scene* sc = SM.GetCurrentScene();
        for (auto o : sc->GetObjects()) {
            if (Player* p = dynamic_cast<Player*>(o)) {
                return p;
            }
        }
        return nullptr;
    }

    void MoveAccordingToPattern(char c) {
        Vector2 vel(0, 0);
        float moveSpeed = 40.0f;
        if (c == '1') vel = Vector2(moveSpeed, 0);
        else if (c == '3') vel = Vector2(0, -moveSpeed);
        else if (c == '5') vel = Vector2(-moveSpeed, 0);
        physics->SetVelocity(vel);
    }

    void WrapScreen() {
        if (transform->position.x < 0)  transform->position.x = (float)RM.WINDOW_WIDTH;
        if (transform->position.x > RM.WINDOW_WIDTH)  transform->position.x = 0;
        if (transform->position.y < 0)  transform->position.y = (float)RM.WINDOW_HEIGHT;
        if (transform->position.y > RM.WINDOW_HEIGHT) transform->position.y = 0;
    }

    void OnCollisionEnter(Object* other) override {
        AM.PlayClip("tank_enemy_seeker_death", 0);
        if (Player* p = dynamic_cast<Player*>(other)) {
            Destroy();
            p->LoseLife();
        }
    }
};