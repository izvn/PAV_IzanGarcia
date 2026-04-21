#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "ExplosionEffect.h" 
#include <iostream>

class ExplodingEnemy : public Enemy {
private:
    std::string pattern;
    float patternTime;
    float patternTimer;
    int currentStep;

    float explosionRadius = 200.0f;  
    float explosionForce = 800.0f;  

public:
    ExplodingEnemy(Vector2 pos)
        : Enemy(GameConfig::GetEnemySkin("enemy_exploding"), Vector2(0, 0), Vector2(32, 64)),
        pattern("135"),
        patternTime(0.5f),
        patternTimer(0),
        currentStep(0)
    {
        transform->position = pos;
        transform->scale = Vector2(1, 1);

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }
        renderer = new AnimatedImageRenderer(
            transform,
            GameConfig::GetEnemySkin("enemy_exploding"),
            Vector2(0, 0),
            32,    
            64,   
            2,   
            0.5f, 
            true 
        );
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
    }

    void MoveAccordingToPattern(char c) {
        Vector2 vel(0, 0);
        float moveSpeed = 100.0f;
        if (c == '1')      vel = Vector2(moveSpeed, 0);
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
        AM.PlayClip("tank_enemy_exploding_death", 0);
        if (Player* p = dynamic_cast<Player*>(other)) {
            Destroy(); 
            p->LoseLife();
        }
    }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        std::cout << "ExplodingEnemy died: spawning explosion effect!\n";

        PushNearbyObjects();

        Enemy::Destroy();
    }

private:
    void PushNearbyObjects() {
        const auto& objs = SM.GetCurrentScene()->GetObjects();
        for (Object* obj : objs) {
            if (dynamic_cast<Bullet*>(obj)) continue;
            if (obj == this) continue;

            Vector2 diff = obj->GetTransform()->position - transform->position;
            float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
            if (dist <= explosionRadius && dist > 0.0f) {
                diff.Normalize();
                float ratio = 1.0f - (dist / explosionRadius);
                float finalForce = explosionForce * ratio;
                obj->GetRigidbody()->AddForce(diff * finalForce);
            }
        }
    }
};

