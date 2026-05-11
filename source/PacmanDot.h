#pragma once
#include "Object.h"
#include "RenderManager.h"

class PacmanDot : public Object {
public:
    PacmanDot(Vector2 pos) {
        transform->position = pos;
        transform->size = Vector2(8, 8);
        transform->scale = Vector2(1.0f, 1.0f);
    }

    void Render() override {
        SDL_Rect rect = {
            (int)(transform->position.x - transform->size.x / 2),
            (int)(transform->position.y - transform->size.y / 2),
            (int)transform->size.x,
            (int)transform->size.y
        };
        SDL_SetRenderDrawColor(RM.GetRenderer(), 255, 255, 0, 255);
        SDL_RenderFillRect(RM.GetRenderer(), &rect);
    }
};