#pragma once
#include "Object.h"
#include <iostream>
#include "AudioManager.h"
#include "ImageObject.h"

class Player : public ImageObject {
protected:
    int lives;
    float shootCooldown;
    float shootTimer;
    bool canShoot;
    bool dead;

public:
    Player(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize)
        : ImageObject(texturePath, sourceOffset, sourceSize),
        lives(3), shootCooldown(0.5f), shootTimer(0),
        canShoot(true), dead(false)
    {
    }

    virtual void Update() override {
        ImageObject::Update(); 
        if (!canShoot) {
            shootTimer += TIME.GetDeltaTime();
            if (shootTimer >= shootCooldown) {
                canShoot = true;
                shootTimer = 0;
            }
        }
    }

    inline bool CanShoot() const { return canShoot && !dead; }
    inline void ResetShoot() { canShoot = false; }

    inline void LoseLife() {
        if (dead) return;
        lives--;
        std::cout << "Player hit! Lives left: " << lives << std::endl;
        if (lives <= 0) {
            dead = true;
            Destroy();
            std::cout << "Player died!\n";
        }
    }

    inline int GetLives() const { return lives; }
    inline bool IsAlive() const { return (lives > 0) && !dead; }

    virtual void HandleInput() = 0;
    virtual void Shoot() = 0;
};
