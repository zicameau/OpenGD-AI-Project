#pragma once
#include <pulse/pulseaudio.h>
#include <alsa/asoundlib.h>
#include <string>
#include <map>
#include <memory>

class LinuxAudioManager {
public:
    static LinuxAudioManager* getInstance();
    
    bool init();
    void cleanup();
    
    // Audio playback control
    int playSound(const std::string& filePath, bool loop = false);
    void stopSound(int soundId);
    void pauseSound(int soundId);
    void resumeSound(int soundId);
    
    // Volume control
    void setVolume(int soundId, float volume);
    void setMasterVolume(float volume);
    
    // State queries
    bool isPlaying(int soundId) const;
    float getCurrentTime(int soundId) const;

private:
    LinuxAudioManager() = default;
    ~LinuxAudioManager();
    
    bool initPulseAudio();
    bool initAlsa();
    void terminatePulseAudio();
    void terminateAlsa();
    
    // PulseAudio context
    pa_mainloop* _mainloop = nullptr;
    pa_context* _context = nullptr;
    
    // ALSA fallback
    snd_pcm_t* _alsaHandle = nullptr;
    
    bool _usePulseAudio = true;
    float _masterVolume = 1.0f;
    
    struct AudioStream {
        std::string filePath;
        bool isLooping;
        float volume;
        bool isPlaying;
        void* streamData;
    };
    
    std::map<int, AudioStream> _activeStreams;
    int _nextStreamId = 1;
    
    static LinuxAudioManager* _instance;
}; 