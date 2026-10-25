#pragma once
#ifndef _SOUNDMANAGER_INCLUDE
#define _SOUNDMANAGER_INCLUDE

#include <../miniaudio.h>
#include <string>

class SoundManager {
public:
    static SoundManager& instance() {
        static SoundManager inst;
        return inst;
    }

    bool init();
    void playSFX(const std::string& filepath);
    void playBGM(const std::string& filepath);
    void stopBGM();
    void cleanUp();

private:
    SoundManager() = default;
    ~SoundManager() = default;

    ma_engine engine;
    ma_sound bgmSound;
    bool isEngineInit = false;
    bool isBGMPlaying = false;
};

#endif