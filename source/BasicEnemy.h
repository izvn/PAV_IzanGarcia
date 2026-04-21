#pragma once
#include "Enemy.h"
#include "Spawner.h"
#include "EnemyBullet.h"
#include "TimeManager.h"
#include "GameConfig.h"
#include "AnimatedImageRenderer.h"
#include "ExplosionEffect.h"

class BasicEnemy : public Enemy {
private:
    float moveSpeed;
    float direction;
    float shootTimer;
    float minShootTime;
    float maxShootTime;

    AnimatedImageRenderer* normalRenderer;
    AnimatedImageRenderer* shootRenderer;

    bool isShooting;
    float shootAnimTimer;
    const float shootAnimDuration = 0.4f; 

public:
    BasicEnemy(Vector2 pos)
        : Enemy(GameConfig::GetEnemySkin("basicEnemy"), Vector2(0, 0), Vector2(32, 64)),
        moveSpeed(100.0f),
        direction(1.0f),
        normalRenderer(nullptr),
        shootRenderer(nullptr),
        isShooting(false),
        shootAnimTimer(0.0f)
    {
        transform->position = pos;
        transform->scale = Vector2(1, 1);

        minShootTime = 2.0f;
        maxShootTime = 5.0f;
        ResetShootTimer();

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        normalRenderer = new AnimatedImageRenderer(
            transform,
            GameConfig::GetEnemySkin("basicEnemy"),
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

    ~BasicEnemy() override {
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

        transform->position.x += direction * moveSpeed * dt;
        if (transform->position.x < 100) {
            transform->position.x = 100;
            direction = 1.0f;
            transform->position.y += 60.0f;
        }
        else if (transform->position.x > 1200) {
            transform->position.x = 1200;
            direction = -1.0f;
            transform->position.y += 60.0f;
        }

        shootTimer -= dt;
        if (shootTimer <= 0) {
            Shoot();
            ResetShootTimer();
        }
    }

    void Shoot() {
        isShooting = true;
        shootAnimTimer = 0.0f;
        renderer = shootRenderer;

        Vector2 bulletPos = transform->position;
        bulletPos.y += (transform->size.y / 2) * transform->scale.y;
        SPAWN.SpawnObject(new EnemyBullet(bulletPos, Vector2(0, 1)));
    }

    void ResetShootTimer() {
        float range = maxShootTime - minShootTime;
        float randomTime = minShootTime + ((float)rand() / RAND_MAX) * range;
        shootTimer = randomTime;
    }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        Enemy::Destroy();
    }
};
