#pragma once
#include "ImageObject.h"
#include "AnimatedImageRenderer.h"
#include "Spawner.h"
#include "TimeManager.h"
#include "GameConfig.h" 

class ExplosionEffect : public ImageObject {
private:
    float lifetime;
    float timer;

public:
    ExplosionEffect(Vector2 pos)
        : ImageObject(GameConfig::GetExplosionSprite(), Vector2(0, 0), Vector2(32, 32)),
        lifetime(0.5f),
        timer(0.0f)
    {
        transform->position = pos;
        transform->scale = Vector2(1, 1);

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        renderer = new AnimatedImageRenderer(
            transform,
            GameConfig::GetExplosionSprite(),
            Vector2(0, 0),
            32,      
            32,      
            2,       
            0.25f,    
            false      
        );
    }

    void Update() override {
        ImageObject::Update();  
        timer += TIME.GetDeltaTime();

        if (timer >= lifetime) {
            Destroy();
        }
    }
};
