#pragma once
#include <SDL.h>
#include <string>
#include <map>
#include <cassert>
#include <SDL_ttf.h>       
#include <unordered_map>  

#define RM RenderManager::GetInstance()

class RenderManager {
public:
    static RenderManager& GetInstance() {
        static RenderManager instance;
        return instance;
    }

    const unsigned int WINDOW_WIDTH = 1360;
    const unsigned int WINDOW_HEIGHT = 768;

    void Init();
    void Release();
    void ClearScreen();
    void RenderScreen();

    SDL_Renderer* GetRenderer() { return renderer; }

    void LoadTexture(std::string path);
    SDL_Texture* GetTexture(std::string path);

    void LoadFont(const std::string& fontPath, int fontSize = 24);
    TTF_Font* GetFont(const std::string& fontPath);

private:
    RenderManager() = default;
    RenderManager(RenderManager&) = delete;
    RenderManager& operator=(const RenderManager&) = delete;
    ~RenderManager();

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::map<std::string, SDL_Texture*> textures;

    std::unordered_map<std::string, TTF_Font*> fonts;

    void InitSDL();
    void CreateWindowAndRenderer();
};
