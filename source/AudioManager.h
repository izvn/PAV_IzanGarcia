#pragma once
#include <SDL_mixer.h>
#include <unordered_map>
#include <string>
#include <cassert>

#define AM AudioManager::GetInstance()

class AudioManager {
private:
    std::unordered_map<std::string, Mix_Chunk*> _clips;
    std::unordered_map<std::string, Mix_Music*> _songs;
    bool _muted = false;

    AudioManager() {
        int result = Mix_OpenAudio(
            44100,             
            MIX_DEFAULT_FORMAT, 
            2, 
            2048
        );

        bool succes = result >= 0;
        if (!succes) {
            throw SDL_GetError();
        }

        Mix_VolumeMusic(MIX_MAX_VOLUME);
        Mix_Volume(-1, MIX_MAX_VOLUME);
    }

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

public:
    static AudioManager& GetInstance() {
        static AudioManager am;
        return am;
    }

    void LoadClip(const std::string& name) {
        assert(_clips.find(name) == _clips.end());
        Mix_Chunk* loadedChunk = Mix_LoadWAV(("resources/audio/sfx/" + name + ".wav").c_str());
        assert(loadedChunk != nullptr);
        _clips.emplace(name, loadedChunk);
    }

    void LoadSongs(const std::string& name) {
        assert(_songs.find(name) == _songs.end());
        Mix_Music* loadedMusic = Mix_LoadMUS(("resources/audio/music/" + name + ".mp3").c_str());
        assert(loadedMusic != nullptr);
        _songs.emplace(name, loadedMusic);
    }

    void PlayClip(const std::string& name, int repetitions) {
        assert(_clips.find(name) != _clips.end());
        Mix_PlayChannel(-1, _clips[name], repetitions);
    }

    void PlaySong(const std::string& name) {
        assert(_songs.find(name) != _songs.end());
        Mix_FadeInMusic(_songs[name], -1, 200);
    }

    void StopMusic() {
        Mix_HaltMusic();
        Mix_HaltChannel(-1);
    }

    void Mute() {
        _muted = true;
        Mix_VolumeMusic(0);
        Mix_Volume(-1, 0);
    }

    void Unmute() {
        _muted = false;
        Mix_VolumeMusic(MIX_MAX_VOLUME);
        Mix_Volume(-1, MIX_MAX_VOLUME);
    }

    bool GetMuted() const {
        return _muted;
    }
};
