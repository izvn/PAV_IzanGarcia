#pragma once
#include "Object.h"
#include "RenderManager.h"

class PacmanWall : public Object {
public:
    PacmanWall(Vector2 pos, Vector2 size) {
        transform->position = pos;
        transform->size = size;
        transform->scale = Vector2(1.0f, 1.0f);
    }

    void Render() override {
        SDL_Rect rect = {
            (int)(transform->position.x - transform->size.x / 2),
            (int)(transform->position.y - transform->size.y / 2),
            (int)transform->size.x,
            (int)transform->size.y
        };
        SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 50, 255, 255);
        SDL_RenderFillRect(RM.GetRenderer(), &rect);
    }
};