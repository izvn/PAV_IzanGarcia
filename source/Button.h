#pragma once
#include <SDL.h>
#include <string>
#include "RenderManager.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "Vector2.h"
#include <SDL_ttf.h>

enum class ButtonState {
    NORMAL,
    HOVER,
    PRESSED
};

class Button {
private:
    SDL_Rect bounds;
    std::string label;
    ButtonState state;
    bool clicked;

    TTF_Font* font;
    SDL_Texture* textTexture;
    int textW, textH;

    bool hoverSoundPlayed; 

public:
    Button(const std::string& lbl, int x, int y, int w, int h, TTF_Font* f)
        : label(lbl), clicked(false), state(ButtonState::NORMAL),
        font(f), textTexture(nullptr), textW(0), textH(0), hoverSoundPlayed(false)
    {
        bounds = { x, y, w, h };
        CreateTextTexture();
    }

    ~Button() {
        if (textTexture) {
            SDL_DestroyTexture(textTexture);
            textTexture = nullptr;
        }
    }

    void Update() {
        int mx = Input.GetMouseX();
        int my = Input.GetMouseY();
        bool inside = (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h);

        if (inside) {
            if (Input.GetLeftClick()) {
                if (state != ButtonState::PRESSED) {
                    state = ButtonState::PRESSED;
                    clicked = true;
                    AM.PlayClip("button_click", 0); 
                }
            }
            else {
                if (state != ButtonState::HOVER) {
                    state = ButtonState::HOVER;
                    if (!hoverSoundPlayed) {
                        AM.PlayClip("button_hover", 0); 
                        hoverSoundPlayed = true;
                    }
                }
                clicked = false;
            }
        }
        else {
            state = ButtonState::NORMAL;
            hoverSoundPlayed = false; 
            clicked = false;
        }
    }

    void Render() {
        SDL_Color color;
        switch (state) {
        case ButtonState::NORMAL:   color = { 80, 80, 80, 255 };   break;
        case ButtonState::HOVER:    color = { 120, 120, 120, 255 }; break;
        case ButtonState::PRESSED:  color = { 160, 160, 160, 255 }; break;
        }

        SDL_SetRenderDrawColor(RM.GetRenderer(), color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(RM.GetRenderer(), &bounds);

        if (textTexture) {
            SDL_Rect textRect;
            textRect.w = textW;
            textRect.h = textH;
            textRect.x = bounds.x + (bounds.w - textW) / 2;
            textRect.y = bounds.y + (bounds.h - textH) / 2;

            SDL_RenderCopy(RM.GetRenderer(), textTexture, nullptr, &textRect);
        }
    }

    bool IsClicked() {
        if (clicked) {
            clicked = false; 
            return true;
        }
        return false;
    }

    std::string GetLabel() const {
        return label;
    }

    void SetLabel(const std::string& newLabel) {
        label = newLabel;
        CreateTextTexture();
    }

private:
    void CreateTextTexture() {
        if (!font) return;

        if (textTexture) {
            SDL_DestroyTexture(textTexture);
            textTexture = nullptr;
        }

        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Surface* surf = TTF_RenderText_Blended(font, label.c_str(), white);
        if (!surf) return;

        textTexture = SDL_CreateTextureFromSurface(RM.GetRenderer(), surf);
        textW = surf->w;
        textH = surf->h;
        SDL_FreeSurface(surf);
    }
};
