#pragma once
#include <queue>
#include "Object.h"

#define SPAWN Spawner::Instance()

class Spawner {
private:
    std::queue<Object*> spawnedObjects;

    Spawner() = default;

public:
    static Spawner& Instance() {
        static Spawner sp;
        return sp;
    }

    void SpawnObject(Object* obj) {
        spawnedObjects.push(obj);
    }

    Object* GetSpawnedObject() {
        if (spawnedObjects.empty()) return nullptr;
        Object* o = spawnedObjects.front();
        spawnedObjects.pop();
        return o;
    }

    int GetSpawnedObjectsCount() {
        return (int)spawnedObjects.size();
    }

    void ClearSpawnedObjects() {
        while (!spawnedObjects.empty()) {
            delete spawnedObjects.front();
            spawnedObjects.pop();
        }
    }
};
