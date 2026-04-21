#pragma once
#include "SDL.h"
#include "Transform.h"
#include <string>

class Renderer {
protected:
    SDL_Color color;
    Transform* transform;
    SDL_Rect sourceRect;
    SDL_Rect destRect;
    std::string targetPath;

public:
    Renderer(Transform* transform, std::string targetPath)
        : transform(transform), targetPath(targetPath)
    {
        color = { 255, 255, 255, 255 };
    }

    virtual ~Renderer() {}

    virtual void Update(float dt) = 0;
    virtual void Render() = 0;

    virtual void SetColor(SDL_Color c) {
        color = c;
    }
};
