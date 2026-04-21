#pragma once
#include "Enemy.h"
#include "Spawner.h"
#include "EnemyBullet.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "ExplosionEffect.h" 
#include <cmath>

class ShootingEnemy : public Enemy {
private:
    std::string pattern;
    float patternTime;
    float patternTimer;
    int currentStep;
    float shootTimer;
    float shootInterval;
    Vector2 facingDir;

    AnimatedImageRenderer* normalRenderer;
    AnimatedImageRenderer* shootRenderer;

    bool isShooting;            
    float shootAnimTimer;       
    const float shootAnimDuration = 0.4f; 

public:
    ShootingEnemy(Vector2 pos)
        : Enemy(GameConfig::GetEnemySkin("enemy_shooting"), Vector2(0, 0), Vector2(32, 64)),
        pattern("135"),
        patternTime(0.8f),
        patternTimer(0),
        currentStep(0),
        shootTimer(0),
        shootInterval(2.0f),
        facingDir(1, 0),
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
            GameConfig::GetEnemySkin("enemy_shooting"),
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
            0.2f,  
            false 
        );

        renderer = normalRenderer;
    }

    ~ShootingEnemy() override {
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

        patternTimer += dt;
        shootTimer += dt;
        if (patternTimer >= patternTime) {
            patternTimer = 0;
            currentStep = (currentStep + 1) % pattern.size();
            MoveAccordingToPattern(pattern[currentStep]);
        }
        if (shootTimer >= shootInterval) {
            shootTimer = 0;
            Shoot();
        }

        WrapScreen();
    }

    void MoveAccordingToPattern(char c) {
        Vector2 vel(0, 0);
        float moveSpeed = 70.0f;
        if (c == '1') {
            vel = Vector2(moveSpeed, 0);
            facingDir = Vector2(1, 0);
        }
        else if (c == '3') {
            vel = Vector2(0, -moveSpeed);
            facingDir = Vector2(0, -1);
        }
        else if (c == '5') {
            vel = Vector2(-moveSpeed, 0);
            facingDir = Vector2(-1, 0);
        }
        physics->SetVelocity(vel);
    }

    void Shoot() {
        isShooting = true;
        shootAnimTimer = 0.0f;
        renderer = shootRenderer;

        Vector2 bulletPos = transform->position;
        bulletPos.x += facingDir.x * (transform->size.x / 2);
        bulletPos.y += facingDir.y * (transform->size.y / 2);
        SPAWN.SpawnObject(new EnemyBullet(bulletPos, facingDir));
    }

    void WrapScreen() {
        if (transform->position.x < 0) transform->position.x = (float)RM.WINDOW_WIDTH;
        if (transform->position.x > RM.WINDOW_WIDTH) transform->position.x = 0;
        if (transform->position.y < 0) transform->position.y = (float)RM.WINDOW_HEIGHT;
        if (transform->position.y > RM.WINDOW_HEIGHT) transform->position.y = 0;
    }

    void OnCollisionEnter(Object* other) override {
        AM.PlayClip("tank_enemy_shooting_death", 0);
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
