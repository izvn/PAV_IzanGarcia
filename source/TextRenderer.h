#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include "Renderer.h"
#include "RenderManager.h"
#include "Transform.h"
#include <cassert>

#define FONT_PATH "resources/fonts/fuente.otf"

class TextRenderer : public Renderer {
protected:
    std::string text;
    SDL_Texture* textTexture = nullptr;
    bool autoSize = true;

public:
    TextRenderer(Transform* transform, const std::string& _text)
        : Renderer(transform, FONT_PATH) 
    {
        SetText(_text);
    }

    void SetColor(SDL_Color color) override {
        Renderer::SetColor(color);
        SetText(text); 
    }

    void Update(float dt) override {
        Vector2 offset = (Vector2(-transform->size.x, -transform->size.y) / 2.0f) * transform->scale;

        destRect.x = (int)(transform->position.x + offset.x);
        destRect.y = (int)(transform->position.y + offset.y);

        if (autoSize) {
            destRect.w = (int)(sourceRect.w * transform->scale.x);
            destRect.h = (int)(sourceRect.h * transform->scale.y);
        }
        else {
            destRect.w = (int)(transform->size.x * transform->scale.x);
            destRect.h = (int)(transform->size.y * transform->scale.y);
        }
    }

    void Render() override {
        if (!textTexture) return;
        SDL_RenderCopyEx(RM.GetRenderer(), textTexture, &sourceRect, &destRect,
            transform->rotation, NULL, SDL_FLIP_NONE);
    }

    void SetText(const std::string& newText) {
        RM.LoadFont(targetPath, 24);

        if (textTexture) {
            SDL_DestroyTexture(textTexture);
            textTexture = nullptr;
        }

        TTF_Font* font = RM.GetFont(targetPath);
        if (!font) {
            SDL_Log("TextRenderer::SetText() - No font loaded for %s", targetPath.c_str());
            return;
        }

        SDL_Surface* surf = TTF_RenderText_Solid(font, newText.c_str(), color);
        assert(surf);

        textTexture = SDL_CreateTextureFromSurface(RM.GetRenderer(), surf);
        assert(textTexture);

        sourceRect = {
            0,
            0,
            surf->w,
            surf->h
        };

        SDL_FreeSurface(surf);

        text = newText; 
    }

    ~TextRenderer() override {
        if (textTexture) {
            SDL_DestroyTexture(textTexture);
            textTexture = nullptr;
        }
    }
};
