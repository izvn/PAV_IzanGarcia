#include "RenderManager.h"
#include <iostream>
#include <SDL_image.h>

void RenderManager::Init() {
    try {
        InitSDL();
        CreateWindowAndRenderer();
    }
    catch (std::exception& exception) {
        std::cout << exception.what() << std::endl;
        SDL_Quit();
        return;
    }
}

void RenderManager::Release() {
    for (auto& t : textures) {
        SDL_DestroyTexture(t.second);
    }
    textures.clear();

    for (auto& f : fonts) {
        if (f.second) {
            TTF_CloseFont(f.second);
        }
    }
    fonts.clear();

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void RenderManager::ClearScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void RenderManager::RenderScreen() {
    SDL_RenderPresent(renderer);
}

RenderManager::~RenderManager() {
    Release(); 
}

void RenderManager::LoadTexture(std::string path) {
    if (textures.find(path) != textures.end()) return;

    SDL_Surface* surf = IMG_Load(path.c_str());
    if (!surf) {
        std::cout << "Error loading image: " << path << " - " << IMG_GetError() << std::endl;
        return;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (!tex) {
        std::cout << "Error creating texture from: " << path
            << " - " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surf);
        return;
    }

    textures[path] = tex;
    SDL_FreeSurface(surf);
}

SDL_Texture* RenderManager::GetTexture(std::string path) {
    if (textures.find(path) != textures.end()) {
        return textures[path];
    }
    return nullptr;
}

void RenderManager::InitSDL() {
    int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    bool success = (result >= 0);
    if (!success) {
        throw std::runtime_error(SDL_GetError());
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cout << "SDL_image could not initialize! Error: " << IMG_GetError() << std::endl;
    }

    if (TTF_Init() == -1) {
        std::cout << "Error initializing SDL_ttf: " << TTF_GetError() << std::endl;
    }
}

void RenderManager::CreateWindowAndRenderer() {
    int result = SDL_CreateWindowAndRenderer(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN,
        &window,
        &renderer
    );
    bool success = (result >= 0);
    if (!success) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void RenderManager::LoadFont(const std::string& fontPath, int fontSize) {
    if (fonts.find(fontPath) != fonts.end()) return;

    TTF_Font* f = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!f) {
        SDL_Log("Error loading font %s: %s", fontPath.c_str(), TTF_GetError());
        return;
    }
    fonts[fontPath] = f;
}

TTF_Font* RenderManager::GetFont(const std::string& fontPath) {
    if (fonts.find(fontPath) != fonts.end()) {
        return fonts[fontPath];
    }
    return nullptr;
}
