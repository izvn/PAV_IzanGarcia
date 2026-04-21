#pragma once
#include "Vector2.h"

class Transform {
public:
    Vector2 position;
    float rotation;
    Vector2 scale;
    Vector2 size;

    Transform()
        : position(0, 0),
        rotation(0),
        scale(1, 1),
        size(100, 100)
    {
    }
};
