#pragma once
#include "ImageObject.h"
#include "Player.h"
#include "ExplosionEffect.h" 

class Enemy : public ImageObject {
public:
    Enemy(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize)
        : ImageObject(texturePath, sourceOffset, sourceSize)
    {
    }

    virtual bool IsEnemy() const { return true; }

    void Destroy() override {
        SPAWN.SpawnObject(new ExplosionEffect(transform->position));
        ImageObject::Destroy();
    }
};
