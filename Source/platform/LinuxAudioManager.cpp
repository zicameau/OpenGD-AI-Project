#include "LinuxAudioManager.h"
#include <iostream>
#include <pulse/error.h>
#include <cstring>
#include <stdexcept>
#include <syslog.h>

LinuxAudioManager* LinuxAudioManager::_instance = nullptr;

LinuxAudioManager* LinuxAudioManager::getInstance() {
    if (!_instance) {
        _instance = new LinuxAudioManager();
    }
    return _instance;
}

LinuxAudioManager::LinuxAudioManager() {
    // Initialize with PulseAudio as primary, ALSA as fallback
    _usePulseAudio = false;
    _mainloop = nullptr;
    _context = nullptr;
    _stream = nullptr;
    _alsaHandle = nullptr;
}

bool LinuxAudioManager::initializeAudio() {
    std::lock_guard<std::mutex> lock(_audioMutex);
    try {
        // Try PulseAudio first
        if (initializePulseAudio()) {
            _usePulseAudio = true;
            setupDeviceMonitoring();
            return true;
        }

        // Fallback to ALSA
        syslog(LOG_WARNING, "PulseAudio initialization failed, falling back to ALSA");
        if (initializeAlsa()) {
            _usePulseAudio = false;
            return true;
        }

        throw std::runtime_error("Failed to initialize audio system");
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Audio initialization failed: %s", e.what());
        cleanup();
        return false;
    }
}

bool LinuxAudioManager::initializePulseAudio() {
    try {
        _mainloop = pa_threaded_mainloop_new();
        if (!_mainloop) {
            throw std::runtime_error("Failed to create PA mainloop");
        }

        _context = pa_context_new(pa_threaded_mainloop_get_api(_mainloop),
                                "OpenGD");
        if (!_context) {
            throw std::runtime_error("Failed to create PA context");
        }

        pa_context_set_state_callback(_context, 
            [](pa_context* c, void* userdata) {
                auto* manager = static_cast<LinuxAudioManager*>(userdata);
                // Handle context state changes
            }, this);

        if (pa_threaded_mainloop_start(_mainloop) < 0) {
            throw std::runtime_error("Failed to start PA mainloop");
        }

        return true;
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "PulseAudio initialization failed: %s", e.what());
        return false;
    }
}

bool LinuxAudioManager::initializeAlsa() {
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

bool LinuxAudioManager::playSound(const std::string& path) {
    std::lock_guard<std::mutex> lock(_streamMutex);
    try {
        if (_usePulseAudio) {
            // PulseAudio playback implementation
            pa_sample_spec spec = {
                .format = PA_SAMPLE_S16LE,
                .rate = 44100,
                .channels = 2
            };

            _stream = pa_stream_new(_context, "OpenGD Sound", &spec, nullptr);
            if (!_stream) {
                throw std::runtime_error("Failed to create PA stream");
            }

            // Configure buffer attributes
            pa_buffer_attr attr = {
                .maxlength = MAX_BUFFER_SIZE,
                .tlength = MAX_BUFFER_SIZE / 2,
                .prebuf = MAX_BUFFER_SIZE / 4,
                .minreq = MAX_BUFFER_SIZE / 4,
                .fragsize = MAX_BUFFER_SIZE / 4
            };

            if (pa_stream_connect_playback(_stream, nullptr, &attr,
                PA_STREAM_ADJUST_LATENCY, nullptr, nullptr) < 0) {
                throw std::runtime_error("Failed to connect PA stream");
            }
        } else {
            // ALSA playback implementation
            // ... ALSA implementation ...
        }

        return true;
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Sound playback failed: %s", e.what());
        return false;
    }
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
    std::lock_guard<std::mutex> lock(_audioMutex);
    // Stop all active streams
    for (const auto& pair : _activeStreams) {
        stopSound(pair.first);
    }
    _activeStreams.clear();

    if (_usePulseAudio) {
        if (_stream) {
            pa_stream_disconnect(_stream);
            pa_stream_unref(_stream);
            _stream = nullptr;
        }
        if (_context) {
            pa_context_disconnect(_context);
            pa_context_unref(_context);
            _context = nullptr;
        }
        if (_mainloop) {
            pa_threaded_mainloop_stop(_mainloop);
            pa_threaded_mainloop_free(_mainloop);
            _mainloop = nullptr;
        }
    } else if (_alsaHandle) {
        snd_pcm_close(_alsaHandle);
        _alsaHandle = nullptr;
    }
}

LinuxAudioManager::~LinuxAudioManager() {
    cleanup();
} 
