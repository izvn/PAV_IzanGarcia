#pragma once
#include <unordered_map>
#include <string>
#include "Scene.h"

#define SM SceneManager::Instance()

class SceneManager {
private:
    std::unordered_map<std::string, Scene*> scenes;
    Scene* currentScene = nullptr;
    std::string nextScene = "";

    SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

public:
    static SceneManager& Instance() {
        static SceneManager sm;
        return sm;
    }

    bool AddScene(std::string name, Scene* scene) {
        if (scenes.find(name) == scenes.end()) {
            scenes.emplace(name, scene);
            return true;
        }
        return false;
    }

    Scene* GetScene(std::string name) {
        if (scenes.find(name) != scenes.end()) {
            return scenes[name];
        }
        return nullptr;
    }

    bool InitFirstScene(std::string name) {
        if (scenes.find(name) != scenes.end()) {
            currentScene = scenes[name];
            currentScene->OnEnter();
            return true;
        }
        return false;
    }

    Scene* GetCurrentScene() { return currentScene; }

    const std::string& GetNextSceneName() const {
        return nextScene; 
    }

    bool SetNextScene(std::string name) {
        if (scenes.find(name) == scenes.end()) return false;
        nextScene = name;
        return true;
    }

    void UpdateCurrentScene() {
        if (!nextScene.empty()) {
            currentScene->OnExit();

            currentScene = scenes[nextScene];
            currentScene->OnEnter();
            nextScene.clear();
        }
        currentScene->Update();
    }
};
