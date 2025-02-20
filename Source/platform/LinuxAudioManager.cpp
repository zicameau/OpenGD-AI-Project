#include "LinuxAudioManager.h"
#include <iostream>
#include <pulse/error.h>
#include <cstring>

LinuxAudioManager* LinuxAudioManager::_instance = nullptr;

LinuxAudioManager* LinuxAudioManager::getInstance() {
    if (!_instance) {
        _instance = new LinuxAudioManager();
    }
    return _instance;
}

bool LinuxAudioManager::init() {
    // Try PulseAudio first
    if (initPulseAudio()) {
        _usePulseAudio = true;
        std::cout << "Using PulseAudio for sound output" << std::endl;
        return true;
    }
    
    // Fallback to ALSA
    if (initAlsa()) {
        _usePulseAudio = false;
        std::cout << "Using ALSA for sound output" << std::endl;
        return true;
    }
    
    std::cerr << "Failed to initialize audio system" << std::endl;
    return false;
}

bool LinuxAudioManager::initPulseAudio() {
    _mainloop = pa_mainloop_new();
    if (!_mainloop) {
        return false;
    }

    _context = pa_context_new(pa_mainloop_get_api(_mainloop), "OpenGD");
    if (!_context) {
        pa_mainloop_free(_mainloop);
        _mainloop = nullptr;
        return false;
    }

    // Connect to PulseAudio server
    if (pa_context_connect(_context, nullptr, PA_CONTEXT_NOFLAGS, nullptr) < 0) {
        pa_context_unref(_context);
        pa_mainloop_free(_mainloop);
        _context = nullptr;
        _mainloop = nullptr;
        return false;
    }

    // Wait for context to be ready
    pa_context_state_t state;
    do {
        pa_mainloop_iterate(_mainloop, 1, nullptr);
        state = pa_context_get_state(_context);
    } while (state != PA_CONTEXT_READY && state != PA_CONTEXT_FAILED);

    if (state != PA_CONTEXT_READY) {
        cleanup();
        return false;
    }

    return true;
}

bool LinuxAudioManager::initAlsa() {
    int err = snd_pcm_open(&_alsaHandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        std::cerr << "ALSA error: " << snd_strerror(err) << std::endl;
        return false;
    }

    // Configure ALSA parameters
    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(_alsaHandle, params);

    err = snd_pcm_hw_params_set_access(_alsaHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) goto error;

    err = snd_pcm_hw_params_set_format(_alsaHandle, params, SND_PCM_FORMAT_S16_LE);
    if (err < 0) goto error;

    err = snd_pcm_hw_params_set_channels(_alsaHandle, params, 2);
    if (err < 0) goto error;

    unsigned int rate = 44100;
    err = snd_pcm_hw_params_set_rate_near(_alsaHandle, params, &rate, 0);
    if (err < 0) goto error;

    err = snd_pcm_hw_params(_alsaHandle, params);
    if (err < 0) goto error;

    return true;

error:
    std::cerr << "ALSA configuration error: " << snd_strerror(err) << std::endl;
    snd_pcm_close(_alsaHandle);
    _alsaHandle = nullptr;
    return false;
}

int LinuxAudioManager::playSound(const std::string& filePath, bool loop) {
    AudioStream stream;
    stream.filePath = filePath;
    stream.isLooping = loop;
    stream.volume = 1.0f;
    stream.isPlaying = true;
    
    int streamId = _nextStreamId++;
    _activeStreams[streamId] = stream;
    
    if (_usePulseAudio) {
        // PulseAudio playback implementation
        pa_stream* paStream = pa_stream_new(_context, "OpenGD Audio", 
            &pa_sample_spec{
                .format = PA_SAMPLE_S16LE,
                .rate = 44100,
                .channels = 2
            }, nullptr);
            
        if (paStream) {
            _activeStreams[streamId].streamData = paStream;
            // Configure and start stream
            pa_stream_connect_playback(paStream, nullptr, nullptr, 
                PA_STREAM_NOFLAGS, nullptr, nullptr);
        }
    } else {
        // ALSA playback implementation
        // Implementation details for ALSA playback would go here
    }
    
    return streamId;
}

void LinuxAudioManager::stopSound(int soundId) {
    auto it = _activeStreams.find(soundId);
    if (it != _activeStreams.end()) {
        if (_usePulseAudio) {
            pa_stream* stream = static_cast<pa_stream*>(it->second.streamData);
            if (stream) {
                pa_stream_disconnect(stream);
                pa_stream_unref(stream);
            }
        } else {
            // ALSA stop implementation
        }
        _activeStreams.erase(it);
    }
}

void LinuxAudioManager::cleanup() {
    // Stop all active streams
    for (const auto& pair : _activeStreams) {
        stopSound(pair.first);
    }
    _activeStreams.clear();

    if (_usePulseAudio) {
        terminatePulseAudio();
    } else {
        terminateAlsa();
    }
}

void LinuxAudioManager::terminatePulseAudio() {
    if (_context) {
        pa_context_disconnect(_context);
        pa_context_unref(_context);
        _context = nullptr;
    }
    
    if (_mainloop) {
        pa_mainloop_free(_mainloop);
        _mainloop = nullptr;
    }
}

void LinuxAudioManager::terminateAlsa() {
    if (_alsaHandle) {
        snd_pcm_close(_alsaHandle);
        _alsaHandle = nullptr;
    }
}

LinuxAudioManager::~LinuxAudioManager() {
    cleanup();
} 