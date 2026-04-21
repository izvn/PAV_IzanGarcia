#pragma once
#include <cmath>

class Vector2 {
public:
    float x;
    float y;

    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float _x, float _y) : x(_x), y(_y) {}

    void Normalize() {
        float length = std::sqrt(x * x + y * y);
        if (length > 0.0f) {
            x /= length;
            y /= length;
        }
    }

    inline Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    inline Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    inline Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    inline Vector2 operator*(const Vector2& other) const {
        return Vector2(x * other.x, y * other.y);
    }

    inline Vector2 operator/(float scalar) const {
        return Vector2(x / scalar, y / scalar);
    }
};
