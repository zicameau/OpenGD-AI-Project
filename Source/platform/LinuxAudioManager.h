#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <queue>
#include <pulse/pulseaudio.h>
#include <alsa/asoundlib.h>

class LinuxAudioManager {
public:
    static LinuxAudioManager* getInstance() {
        static std::once_flag flag;
        static LinuxAudioManager* instance = nullptr;
        
        std::call_once(flag, []() {
            instance = new LinuxAudioManager();
        });
        
        return instance;
    }

    bool initializeAudio();
    bool playSound(const std::string& path);
    void stopSound();
    bool setVolume(float volume);
    void cleanup();

    virtual ~LinuxAudioManager();
    
    // Delete copy and move operations
    LinuxAudioManager(const LinuxAudioManager&) = delete;
    LinuxAudioManager& operator=(const LinuxAudioManager&) = delete;
    LinuxAudioManager(LinuxAudioManager&&) = delete;
    LinuxAudioManager& operator=(LinuxAudioManager&&) = delete;

private:
    LinuxAudioManager();
    bool initializePulseAudio();
    bool initializeAlsa();
    void setupDeviceMonitoring();
    bool recoverStream();

    std::mutex _audioMutex;
    std::mutex _streamMutex;
    pa_threaded_mainloop* _mainloop = nullptr;
    pa_context* _context = nullptr;
    pa_stream* _stream = nullptr;
    snd_pcm_t* _alsaHandle = nullptr;
    bool _usePulseAudio = true;
    static constexpr size_t MAX_BUFFER_SIZE = 16384;
}; 