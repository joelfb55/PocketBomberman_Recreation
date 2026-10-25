#define MINIAUDIO_IMPLEMENTATION
#include "SoundManager.h"
#include <iostream>

// FUNCIONES OBJETO
bool SoundManager::init() {
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cerr << "Error al inicializar el motor de audio miniaudio." << std::endl;
        return false;
    }
    isEngineInit = true;
    return true;
}

void SoundManager::cleanUp() {
    if (isBGMPlaying) {
        stopBGM();
    }
    if (isEngineInit) {
        ma_engine_uninit(&engine);
        isEngineInit = false;
    }
}

// EFECTOS DE SONIDO
void SoundManager::playSFX(const std::string& filepath) {
    if (!isEngineInit) return;
    
    ma_engine_play_sound(&engine, filepath.c_str(), NULL);
}

//MUSICA
void SoundManager::playBGM(const std::string& filepath) {
    if (!isEngineInit) return;

    if (isBGMPlaying) {
        stopBGM();
    }

    // MA_SOUND_FLAG_STREAM optimiza la memoria para música larga (MP3/WAV)
    ma_result result = ma_sound_init_from_file(&engine, filepath.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, &bgmSound);
    if (result == MA_SUCCESS) {
        ma_sound_set_looping(&bgmSound, MA_TRUE);
        ma_sound_start(&bgmSound);
        isBGMPlaying = true;
    }
}

void SoundManager::stopBGM() {
    if (isBGMPlaying) {
        ma_sound_stop(&bgmSound);
        ma_sound_uninit(&bgmSound);
        isBGMPlaying = false;
    }
}
