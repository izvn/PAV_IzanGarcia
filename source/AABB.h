#pragma once
#include "Vector2.h"

class AABB {
protected:
    Vector2 topLeft;
    Vector2 size;
public:
    AABB(Vector2 _topLeft, Vector2 _size)
        : topLeft(_topLeft), size(_size) {}

    inline Vector2 GetTopLeft() { return topLeft; };
    inline Vector2 GetSize() { return size; }
    inline void SetTopLeft(Vector2 newTopLeft) { topLeft = newTopLeft; }
    inline void SetSize(Vector2 newSize) { size = newSize; };

    bool CheckOverlappingPoint(Vector2 point) {
        bool insideX = point.x >= topLeft.x && point.x <= topLeft.x + size.x;
        bool insideY = point.y >= topLeft.y && point.y <= topLeft.y + size.y;
        return insideX && insideY;
    }

    bool CheckOverlappingAABB(const AABB* other) {
        bool intersectionX = (topLeft.x + size.x >= other->topLeft.x) &&
            (topLeft.x <= other->topLeft.x + other->size.x);
        bool intersectionY = (topLeft.y + size.y >= other->topLeft.y) &&
            (topLeft.y <= other->topLeft.y + other->size.y);
        return intersectionX && intersectionY;
    }
};
