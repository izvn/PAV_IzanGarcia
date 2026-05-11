#pragma once
#include "Bullet.h"

class CentipedePlayerBullet : public Bullet {
public:
    CentipedePlayerBullet(Vector2 startPos)
        : Bullet("resources/bullet.png", startPos, Vector2(16, 16), Vector2(0, -1), 800.0f) {
    }
};