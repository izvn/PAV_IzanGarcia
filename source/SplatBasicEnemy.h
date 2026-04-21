#pragma once
#include "SplatEnemyBase.h"
#include "GameConfig.h"
#include <cstdlib>
#include "TimeManager.h"
#include "AnimatedImageRenderer.h"
#include "ExplosionEffect.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "Player.h"

class SplatBasicEnemy : public SplatEnemyBase {
private:
    std::string pattern;
    float patternTime;
    float patternTimer;
    int currentStep;

    float timeOnPlayer; 
    float damageCooldown; 
    bool canDealDamage; 

public:
    SplatBasicEnemy(Vector2 pos)
        : SplatEnemyBase(GameConfig::GetEnemySkin("enemy_splat"), Vector2(0, 0), Vector2(32, 32)),
        pattern("135"),
        patternTime(1.0f),
        patternTimer(0),
        currentStep(0),
        timeOnPlayer(0),
        damageCooldown(2.0f),
        canDealDamage(true)
    {
        transform->position = pos;
        transform->scale = Vector2(1, 1);
        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }
        renderer = new AnimatedImageRenderer(transform, GameConfig::GetEnemySkin("enemy_splat"), Vector2(0, 0), 32, 32, 2, 0.5f, true);
    }

    void Update() override {
        Enemy::Update();
        float dt = TIME.GetDeltaTime();
        patternTimer += dt;
        if (patternTimer >= patternTime) {
            patternTimer = 0;
            currentStep = (currentStep + 1) % pattern.size();
            MoveAccordingToPattern(pattern[currentStep]);
        }
        WrapScreen();

        if (!canDealDamage) {
            damageCooldown -= dt;
            if (damageCooldown <= 0) {
                canDealDamage = true;
                damageCooldown = 2.0f; 
            }
        }
    }

    void MoveAccordingToPattern(char c) {
        Vector2 vel(0, 0);
        float moveSpeed = 30.0f;
        if (c == '1') vel = Vector2(moveSpeed, 0);
        else if (c == '3') vel = Vector2(0, -moveSpeed);
        else if (c == '5') vel = Vector2(-moveSpeed, 0);
        physics->SetVelocity(vel);
    }

    void WrapScreen() {
        if (transform->position.x < 0) transform->position.x = (float)RM.WINDOW_WIDTH;
        if (transform->position.x > RM.WINDOW_WIDTH) transform->position.x = 0;
        if (transform->position.y < 0) transform->position.y = (float)RM.WINDOW_HEIGHT;
        if (transform->position.y > RM.WINDOW_HEIGHT) transform->position.y = 0;
    }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        SplatEnemyBase::Destroy();
    }

    void OnCollisionEnter(Object* other) override {
        Enemy::OnCollisionEnter(other);
        if (Player* p = dynamic_cast<Player*>(other)) {
            if (!Input.GetLeftClick()) {
                timeOnPlayer += TIME.GetDeltaTime();
                if (timeOnPlayer >= 1.0f && canDealDamage) { 
                    p->LoseLife();
                    timeOnPlayer = 0; 
                    canDealDamage = false; 
                }
            }
            else {
                AM.PlayClip("splat_enemy_death", 0);
                Destroy();
            }
        }
        else {
            timeOnPlayer = 0;
        }
    }
};
