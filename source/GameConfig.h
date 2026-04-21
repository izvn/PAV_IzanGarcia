#pragma once
#include <string>
#include <vector>

class GameConfig {
public:
    static int GetSelectedBackground() { return selectedBackground; }
    static int GetSelectedEnemies() { return selectedEnemies; }
    static int GetSelectedVFX() { return selectedVFX; }

    static void SetSelectedBackground(int index) { selectedBackground = index; }
    static void SetSelectedEnemies(int index) { selectedEnemies = index; }
    static void SetSelectedVFX(int index) { selectedVFX = index; }

    static const std::string& GetBackgroundPath(int index) { return backgroundPaths[index]; }
    static const std::string& GetEnemyPath(int index) { return enemyPaths[index]; }
    static const std::string& GetVFXPath(int index) { return vfxPaths[index]; }

    static std::string GetEnemySkin(const std::string& baseName) {
        if (selectedEnemies == 0) return "resources/" + baseName + ".png";
        if (selectedEnemies == 1) return "resources/" + baseName + "2.png";
        return "resources/" + baseName + "3.png";
    }

    static std::string GetExplosionSprite() {
        if (selectedVFX == 1) {
            return "resources/explosion2.png";
        }
        else if (selectedVFX == 2) {
            return "resources/explosion3.png";
        }
        return "resources/explosion.png"; 
    }

    static std::string GetShootAnimationSprite() {
        if (selectedVFX == 1) {
            return "resources/shoot_animation2.png";
        }
        else if (selectedVFX == 2) {
            return "resources/shoot_animation3.png";
        }
        return "resources/shoot_animation.png";
    }

    static int pendingScore;
    static int pendingMode;

private:
    static int selectedBackground;
    static int selectedEnemies;
    static int selectedVFX;
    static const std::vector<std::string> backgroundPaths;
    static const std::vector<std::string> enemyPaths;
    static const std::vector<std::string> vfxPaths;
};

int GameConfig::selectedBackground = 0;
int GameConfig::selectedEnemies = 0;
int GameConfig::selectedVFX = 0;
int GameConfig::pendingScore = 0;
int GameConfig::pendingMode = 0;

const std::vector<std::string> GameConfig::backgroundPaths = {
    "resources/SpriteBackground1.png",
    "resources/SpriteBackground2.png",
    "resources/SpriteBackground3.png"
};

const std::vector<std::string> GameConfig::enemyPaths = {
    "resources/SpriteEnemies1.png",
    "resources/SpriteEnemies2.png",
    "resources/SpriteEnemies3.png"
};

const std::vector<std::string> GameConfig::vfxPaths = {
    "resources/SpriteVFX1.png",
    "resources/SpriteVFX2.png",
    "resources/SpriteVFX3.png"
};