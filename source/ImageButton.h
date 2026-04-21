#pragma once
#include <SDL.h>
#include <string>
#include "RenderManager.h"
#include "InputManager.h"

enum class ImageButtonState {
    NORMAL,
    HOVER,
    PRESSED
};

class ImageButton {
private:
    SDL_Rect bounds;
    std::string texturePath;
    SDL_Texture* texture;
    ImageButtonState state;
    bool clicked;

public:
    ImageButton(const std::string& path, int x, int y, int w, int h)
        : texturePath(path), texture(nullptr), state(ImageButtonState::NORMAL), clicked(false)
    {
        bounds = { x, y, w, h };
        texture = RM.GetTexture(path);
    }

    void Update() {
        int mx = Input.GetMouseX();
        int my = Input.GetMouseY();
        bool inside = (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h);
        if (inside) {
            if (Input.GetLeftClick()) {
                state = ImageButtonState::PRESSED;
                clicked = true;
            }
            else {
                state = ImageButtonState::HOVER;
                clicked = false;
            }
        }
        else {
            state = ImageButtonState::NORMAL;
            clicked = false;
        }
    }

    void Render() {
        if (!texture) return;
        switch (state) {
        case ImageButtonState::NORMAL:
            SDL_SetTextureColorMod(texture, 255, 255, 255);
            break;
        case ImageButtonState::HOVER:
            SDL_SetTextureColorMod(texture, 200, 200, 200);
            break;
        case ImageButtonState::PRESSED:
            SDL_SetTextureColorMod(texture, 150, 150, 150);
            break;
        }
        SDL_RenderCopy(RM.GetRenderer(), texture, nullptr, &bounds);
        SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 0, 0, 255);
        SDL_RenderDrawRect(RM.GetRenderer(), &bounds);
    }

    bool IsClicked() const {
        return (state == ImageButtonState::PRESSED && clicked);
    }

    std::string GetTexturePath() const {
        return texturePath;
    }
};
