#pragma once
#include "Enemy.h"
#include "GameConfig.h"
#include <cstdlib>
#include "AnimatedImageRenderer.h"

class BasicEnemyTanks : public Enemy {
private:
    std::string pattern;
    float patternTime;
    float patternTimer;
    int currentStep;

public:
    BasicEnemyTanks(Vector2 pos)
        : Enemy(GameConfig::GetEnemySkin("enemy"), Vector2(0, 0), Vector2(32, 32)),
        pattern("135"),
        patternTime(1.0f),
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
            GameConfig::GetEnemySkin("enemy"), 
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
        patternTimer += TIME.GetDeltaTime();
        if (patternTimer >= patternTime) {
            patternTimer = 0;
            currentStep = (currentStep + 1) % pattern.size();
            MoveAccordingToPattern(pattern[currentStep]);
        }
        WrapScreen();
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

    void OnCollisionEnter(Object* other) override {
        AM.PlayClip("tank_enemy_basic_death", 0);
        if (Player* p = dynamic_cast<Player*>(other)) {
            Destroy();
            p->LoseLife();
        }
    }
};
