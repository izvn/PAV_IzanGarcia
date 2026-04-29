#pragma once
#include "Renderer.h"
#include "RenderManager.h"
#include "Transform.h"
#include <string>

class ImageRenderer : public Renderer {
public:
    ImageRenderer(Transform* transform, std::string resourcePath,
        Vector2 sourceOffset, Vector2 sourceSize)
        : Renderer(transform, resourcePath)
    {
        sourceRect = SDL_Rect{
            (int)sourceOffset.x,
            (int)sourceOffset.y,
            (int)sourceSize.x,
            (int)sourceSize.y
        };
        destRect = { 0, 0, 0, 0 };
    }

    virtual void Update(float dt) override {
        Vector2 offset = (Vector2(-transform->size.x, -transform->size.y) / 2.0f) * transform->scale;
        destRect.x = (int)(transform->position.x + offset.x);
        destRect.y = (int)(transform->position.y + offset.y);
        destRect.w = (int)(transform->size.x * transform->scale.x);
        destRect.h = (int)(transform->size.y * transform->scale.y);
    }

    virtual void Render() override {
        SDL_Texture* tex = RM.GetTexture(targetPath);
        if (!tex) {
            SDL_Log("No texture found for %s\n", targetPath.c_str());
            return;
        }
        SDL_RenderCopyEx(RM.GetRenderer(), tex, &sourceRect, &destRect, transform->rotation, nullptr, SDL_FLIP_NONE);
    }
};