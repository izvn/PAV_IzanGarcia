#pragma once
#include <vector>
#include "AABB.h"
#include "Transform.h"

class Rigidbody {
private:
    std::vector<AABB*> colliders;
    Transform* transform;
    Vector2 velocity;
    float angularVelocity;
    Vector2 acceleration;
    float angularAcceleration;
    float linearDrag;
    float angularDrag;

public:
    Rigidbody(Transform* _transform)
        : transform(_transform), velocity(), angularVelocity(0),
        acceleration(), angularAcceleration(0),
        linearDrag(0), angularDrag(0)
    {
    }

    ~Rigidbody() {
        for (AABB* c : colliders) {
            delete c;
        }
    }

    void AddCollider(AABB* collider) {
        colliders.push_back(collider);
    }

    bool CheckCollision(const Rigidbody* other) {
        for (AABB* myCol : colliders) {
            for (AABB* otherCol : other->colliders) {
                if (myCol->CheckOverlappingAABB(otherCol)) {
                    return true;
                }
            }
        }
        return false;
    }

    void Update(float dt) {
        velocity = velocity + acceleration * dt;
        angularVelocity = angularVelocity + angularAcceleration * dt;
        velocity = velocity * (1.0f / (1.0f + dt * linearDrag));
        angularVelocity = angularVelocity * (1.0f / (1.0f + dt * angularDrag));

        transform->position = transform->position + velocity * dt;
        transform->rotation = transform->rotation + angularVelocity * dt;

        acceleration = Vector2();
        angularAcceleration = 0.0f;

        Vector2 offset = (Vector2(-transform->size.x, -transform->size.y) / 2.0f) * transform->scale;
        for (AABB* col : colliders) {
            col->SetTopLeft(transform->position + offset);
            col->SetSize(transform->size * transform->scale);
        }
    }

    inline void AddForce(Vector2 force) { acceleration = acceleration + force; }
    inline void SetVelocity(Vector2 v) { velocity = v; }
    inline Vector2 GetVelocity() const { return velocity; }
    inline void SetLinearDrag(float ld) { linearDrag = ld; }
    inline void SetAngularDrag(float ad) { angularDrag = ad; }
};
