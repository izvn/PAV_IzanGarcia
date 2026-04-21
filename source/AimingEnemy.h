#pragma once
#include "Enemy.h"
#include "Spawner.h"
#include "BulletAimingEnemy.h"
#include "Player.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "ExplosionEffect.h"
#include <cmath>
#include "Scene.h"
#include "SceneManager.h"

class AimingEnemy : public Enemy {
private:
    std::string pattern;
    float patternTime;
    float patternTimer;
    int currentStep;

    float actionTimer;
    float stopTimer;
    float stopDuration;
    float shootInterval;
    bool stopping;

    AnimatedImageRenderer* normalRenderer;
    AnimatedImageRenderer* shootRenderer;

    bool isShooting;         
    float shootAnimTimer;       
    const float shootAnimDuration = 0.5f; 

public:
    AimingEnemy(Vector2 pos)
        : Enemy(GameConfig::GetEnemySkin("enemy_aiming"), Vector2(0, 0), Vector2(32, 64)),
        pattern("135"),
        patternTime(0.8f),
        patternTimer(0),
        currentStep(0),
        actionTimer(0),
        stopTimer(0),
        stopDuration(3.0f),
        shootInterval(5.0f),
        stopping(false),
        normalRenderer(nullptr),
        shootRenderer(nullptr),
        isShooting(false),
        shootAnimTimer(0.0f)
    {
        transform->position = pos;
        transform->scale = Vector2(1, 1);

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        normalRenderer = new AnimatedImageRenderer(
            transform,
            GameConfig::GetEnemySkin("enemy_aiming"),
            Vector2(0, 0),
            32,   
            64, 
            2,   
            0.5f, 
            true  
        );

        shootRenderer = new AnimatedImageRenderer(
            transform,
            GameConfig::GetShootAnimationSprite(), 
            Vector2(0, 0),
            32,    
            64,    
            2,     
            0.25f,  
            false   
        );

        renderer = normalRenderer;
    }

    ~AimingEnemy() override {
        if (renderer == normalRenderer) renderer = nullptr;
        if (renderer == shootRenderer)  renderer = nullptr;

        if (normalRenderer) { delete normalRenderer; normalRenderer = nullptr; }
        if (shootRenderer) { delete shootRenderer;  shootRenderer = nullptr; }
    }

    void Update() override {
        Enemy::Update();
        float dt = TIME.GetDeltaTime();

        if (isShooting) {
            shootAnimTimer += dt;
            if (shootAnimTimer >= shootAnimDuration) {
                isShooting = false;
                shootAnimTimer = 0.0f;
                renderer = normalRenderer; 
            }
        }

        actionTimer += dt;
        if (!stopping) {
            patternTimer += dt;
            if (patternTimer >= patternTime) {
                patternTimer = 0;
                currentStep = (currentStep + 1) % pattern.size();
                MoveAccordingToPattern(pattern[currentStep]);
            }
            if (actionTimer >= shootInterval) {
                stopping = true;
                stopTimer = 0;
                physics->SetVelocity(Vector2(0, 0));
            }
        }
        else {
            stopTimer += dt;
            if (stopTimer >= stopDuration) {
                ShootToPlayer();
                stopping = false;
                stopTimer = 0;
                actionTimer = 0;
            }
        }

        WrapScreen();
    }

    void MoveAccordingToPattern(char c) {
        Vector2 vel(0, 0);
        float moveSpeed = 60.0f;
        if (c == '1')      vel = Vector2(moveSpeed, 0);
        else if (c == '3') vel = Vector2(0, -moveSpeed);
        else if (c == '5') vel = Vector2(-moveSpeed, 0);
        physics->SetVelocity(vel);
    }

    void ShootToPlayer() {
        isShooting = true;
        shootAnimTimer = 0.0f;
        renderer = shootRenderer;

        Player* player = FindPlayer();
        if (!player) return;
        Vector2 playerPos = player->GetTransform()->position;
        Vector2 startPos = transform->position;
        SPAWN.SpawnObject(new BulletAimingEnemy(startPos, playerPos));
    }

    Player* FindPlayer() {
        Scene* sc = SM.GetCurrentScene();
        for (Object* o : sc->GetObjects()) {
            if (Player* p = dynamic_cast<Player*>(o)) {
                return p;
            }
        }
        return nullptr;
    }

    void WrapScreen() {
        if (transform->position.x < 0) transform->position.x = (float)RM.WINDOW_WIDTH;
        if (transform->position.x > RM.WINDOW_WIDTH) transform->position.x = 0;
        if (transform->position.y < 0) transform->position.y = (float)RM.WINDOW_HEIGHT;
        if (transform->position.y > RM.WINDOW_HEIGHT) transform->position.y = 0;
    }

    void OnCollisionEnter(Object* other) override {
        AM.PlayClip("tank_enemy_aiming_death", 0);
        if (Player* p = dynamic_cast<Player*>(other)) {
            Destroy(); 
            p->LoseLife();
        }
    }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        Enemy::Destroy();
    }
};