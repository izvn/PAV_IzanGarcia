#pragma once
#include "ImageObject.h"

class Bullet : public ImageObject {
protected:
    float lifetime;
    float aliveTime;
    Vector2 direction;
    float speed;

public:
    
    Bullet(std::string texturePath, Vector2 startPos, Vector2 srcSize, Vector2 dir, float spd = 300.0f)
        : ImageObject(texturePath, Vector2(0, 0), srcSize),
        lifetime(3.0f),
        aliveTime(0.0f),
        direction(dir),
        speed(spd)
    {
        transform->position = startPos;
        transform->scale = Vector2(1, 1);
        physics->SetVelocity(direction * speed);
    }

    virtual void Update() override {
        ImageObject::Update(); 

        aliveTime += TIME.GetDeltaTime();
        if (aliveTime >= lifetime) {
            Destroy();
        }
    }
};
