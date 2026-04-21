#pragma once
#include <vector>
#include "Object.h"
#include "Spawner.h"

class Scene {
protected:
    std::vector<Object*> objects;

public:
    Scene() = default;
    virtual ~Scene() = default;

    virtual void OnEnter() = 0;
    virtual void OnExit() {
        SPAWN.ClearSpawnedObjects();
        for (Object* o : objects) {
            delete o;
        }
        objects.clear();
    }

    virtual void Update() {
        for (int i = (int)objects.size() - 1; i >= 0; i--) {
            if (objects[i]->IsPendingDestroy()) {
                delete objects[i];
                objects.erase(objects.begin() + i);
            }
        }

        while (SPAWN.GetSpawnedObjectsCount() > 0) {
            objects.push_back(SPAWN.GetSpawnedObject());
        }

        for (Object* o : objects) {
            o->Update();
        }
    }

    virtual void Render() {
        for (Object* o : objects) {
            o->Render();
        }
    }

    const std::vector<Object*>& GetObjects() const {
        return objects;
    }
};
