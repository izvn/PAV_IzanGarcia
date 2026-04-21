#pragma once
#include "Rigidbody.h"
#include "Transform.h"
#include "Renderer.h"
#include "TimeManager.h"


class Object {
private:
    bool isPendingDestroy = false;

protected:
    Transform* transform;
    Rigidbody* physics;
    Renderer* renderer = nullptr;

public:
    Object() {
        transform = new Transform();
        physics = new Rigidbody(transform);
    }

    virtual ~Object() {
        delete transform;
        delete physics;
        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }
    }

    virtual void Update() {
        if (physics) {
            physics->Update(TIME.GetDeltaTime());
        }
        if (renderer) {
            renderer->Update(TIME.GetDeltaTime());
        }
    }

    virtual void Render() {
        if (renderer) {
            renderer->Render();
        }
    }

    Transform* GetTransform() { return transform; }
    Rigidbody* GetRigidbody() { return physics; }

    inline bool IsPendingDestroy() const {
        return isPendingDestroy;
    }

    virtual void Destroy() {
        isPendingDestroy = true;
    }

    virtual void OnCollisionEnter(Object* other) {
    }
};
