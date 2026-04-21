#pragma once
#include "Enemy.h"

class SplatEnemyBase : public Enemy {
public:
    SplatEnemyBase(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize)
        : Enemy(texturePath, sourceOffset, sourceSize) {}

    void OnCollisionEnter(Object* other) override {    }
};
