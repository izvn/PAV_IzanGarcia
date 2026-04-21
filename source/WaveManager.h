#pragma once
#include <vector>
#include "Object.h"
#include "BasicEnemy.h"

class WaveManager {
private:
    int currentWave;
    int totalWaves;
    bool waveCompleted;

public:
    WaveManager() : currentWave(0), totalWaves(1), waveCompleted(false) {}

    void StartFirstWave() {
        currentWave = 1;
        SpawnWave(currentWave);
    }

    bool IsWaveCompleted() const { return waveCompleted; }
    bool IsLastWave() const { return currentWave == totalWaves; }

    void Update() {
      
    }

    void SpawnWave(int waveNumber) {
       
        for (int i = 0; i < 5; i++) {
            Vector2 pos(200 + i * 100, 100);
            SPAWN.SpawnObject(new BasicEnemy(pos));
        }
        waveCompleted = false;
    }

    void NextWave() {
        if (currentWave < totalWaves) {
            currentWave++;
            SpawnWave(currentWave);
        }
    }

    void SetWaveCompleted() { waveCompleted = true; }
};
