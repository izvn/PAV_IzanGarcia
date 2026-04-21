#pragma once
#include "ImageObject.h"
#include <cstdlib>

class TestObject : public ImageObject {
public:
    TestObject()
        : ImageObject("resources/prueba.png", Vector2(0.f, 0.f), Vector2(306.f, 562.f))
    {
        Vector2 randomPos = Vector2(rand() % RM.WINDOW_WIDTH, rand() % RM.WINDOW_HEIGHT);
        transform->position = randomPos;
        transform->scale = Vector2(0.2f, 0.2f);

        Vector2 randomForce = Vector2(-500 + rand() % 1001, -500 + rand() % 1001);
        physics->AddForce(randomForce);
    }

    TestObject(Vector2 pos, Vector2 startVelocity)
        : ImageObject("resources/prueba.png", Vector2(0.f, 0.f), Vector2(306.f, 562.f))
    {
        transform->position = pos;
        transform->scale = Vector2(0.2f, 0.2f);
        physics->AddForce(startVelocity);
    }

    void OnCollisionEnter(Object* other) override {
        Destroy();
    }
};
