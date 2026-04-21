#pragma once
#include "RenderManager.h"
#include "Transform.h"

class GameObject {
protected:
    Transform* transform;
    SDL_Texture* texture = nullptr;
    SDL_Rect sourceRect = { 0, 0, 0, 0 };
    SDL_Rect destRect = { 0, 0, 0, 0 };

public:
    GameObject(const std::string& texturePath, Transform* _transform)
        : transform(_transform) {
        RM.LoadTexture(texturePath);
        texture = RM.GetTexture(texturePath);
        assert(texture && "Failed to load texture!");

        SDL_QueryTexture(texture, nullptr, nullptr, &sourceRect.w, &sourceRect.h);
        destRect.w = sourceRect.w;
        destRect.h = sourceRect.h;
    }

    virtual void Update(float dt) {
        destRect.x = static_cast<int>(transform->position.x);
        destRect.y = static_cast<int>(transform->position.y);
        destRect.w = static_cast<int>(transform->size.x);
        destRect.h = static_cast<int>(transform->size.y);
    }

    virtual void Render() {
        if (texture) {
            SDL_RenderCopyEx(RM.GetRenderer(), texture, &sourceRect, &destRect, transform->rotation, nullptr, SDL_FLIP_NONE);
        }
    }

    virtual ~GameObject() = default;
};
