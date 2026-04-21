#pragma once

#include "Scene.h"
#include "Spawner.h"
#include "TestObject.h"
#include "AudioManager.h"

class Gameplay : public Scene {
public:
    Gameplay() = default;

    void OnEnter() override {
        SPAWN.SpawnObject(new TestObject(Vector2(100, 100), Vector2(1000, 0)));
        SPAWN.SpawnObject(new TestObject(Vector2(300, 100), Vector2(-1000, 0)));
    }

    void SpawnObjectRandomly() {
        Object* o = new Object(); 
        SPAWN.SpawnObject(o);
    }
};
