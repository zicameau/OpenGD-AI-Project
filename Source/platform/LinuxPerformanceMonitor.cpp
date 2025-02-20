#include "LinuxPerformanceMonitor.h"
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <fstream>
#include <thread>
#include <cstring>

LinuxPerformanceMonitor* LinuxPerformanceMonitor::getInstance() {
    static LinuxPerformanceMonitor instance;
    return &instance;
}

void LinuxPerformanceMonitor::beginFrame() {
    _frameStart = std::chrono::high_resolution_clock::now();
}

void LinuxPerformanceMonitor::endFrame() {
    auto frameEnd = std::chrono::high_resolution_clock::now();
    float frameTime = std::chrono::duration<float>(frameEnd - _frameStart).count();
    
    _frameTimeHistory.push_back(frameTime);
    if (_frameTimeHistory.size() > 100) {
        _frameTimeHistory.erase(_frameTimeHistory.begin());
    }
    
    float currentFPS = 1.0f / frameTime;
    _fpsHistory.push_back(currentFPS);
    if (_fpsHistory.size() > 100) {
        _fpsHistory.erase(_fpsHistory.begin());
    }

    if (_loggingEnabled) {
        PerformanceMetrics metrics;
        metrics.fps = currentFPS;
        metrics.frameTime = frameTime;
        metrics.cpuUsage = getCPUUsage();
        metrics.memoryUsage = getMemoryUsage();
        metrics.gpuUsage = getGPUUsage();
        
        _metricsHistory.push_back(metrics);
    }
}

float LinuxPerformanceMonitor::getCurrentFPS() const {
    if (_fpsHistory.empty()) return 0.0f;
    return _fpsHistory.back();
}

float LinuxPerformanceMonitor::getAverageFrameTime() const {
    if (_frameTimeHistory.empty()) return 0.0f;
    float sum = 0.0f;
    for (float time : _frameTimeHistory) {
        sum += time;
    }
    return sum / _frameTimeHistory.size();
}

float LinuxPerformanceMonitor::getCPUUsage() const {
    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open()) return 0.0f;

    std::string line;
    std::getline(statFile, line);
    
    unsigned long user, nice, system, idle;
    sscanf(line.c_str(), "cpu %lu %lu %lu %lu", &user, &nice, &system, &idle);
    
    unsigned long total = user + nice + system + idle;
    return 100.0f * (1.0f - (float)idle / total);
}

float LinuxPerformanceMonitor::getMemoryUsage() const {
    struct sysinfo si;
    if (sysinfo(&si) != 0) return 0.0f;
    
    return 100.0f * (1.0f - (float)si.freeram / si.totalram);
}

float LinuxPerformanceMonitor::getGPUUsage() const {
    // This would need to be implemented differently for different GPU vendors
    // Example for NVIDIA using nvidia-smi
    FILE* pipe = popen("nvidia-smi --query-gpu=utilization.gpu --format=csv,noheader,nounits", "r");
    if (!pipe) return 0.0f;
    
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }
    pclose(pipe);
    
    float usage = 0.0f;
    sscanf(result.c_str(), "%f", &usage);
    return usage;
}

void LinuxPerformanceMonitor::beginProfile(const std::string& name) {
    _profileStartTimes[name] = std::chrono::high_resolution_clock::now();
}

void LinuxPerformanceMonitor::endProfile(const std::string& name) {
    auto it = _profileStartTimes.find(name);
    if (it != _profileStartTimes.end()) {
        auto duration = std::chrono::high_resolution_clock::now() - it->second;
        _profilingResults[name] = std::chrono::duration<float>(duration).count();
    }
}

std::unordered_map<std::string, float> LinuxPerformanceMonitor::getProfilingResults() const {
    return _profilingResults;
}

void LinuxPerformanceMonitor::enableLogging(bool enable) {
    _loggingEnabled = enable;
    if (!enable) {
        _metricsHistory.clear();
    }
}

void LinuxPerformanceMonitor::setLogInterval(float seconds) {
    _logInterval = seconds;
}

std::vector<PerformanceMetrics> LinuxPerformanceMonitor::getPerformanceHistory() const {
    return _metricsHistory;
}

std::vector<std::string> LinuxPerformanceMonitor::getOptimizationSuggestions() const {
    std::vector<std::string> suggestions;
    
    float avgFPS = 0.0f;
    for (float fps : _fpsHistory) {
        avgFPS += fps;
    }
    avgFPS /= _fpsHistory.size();
    
    if (avgFPS < 60.0f) {
        suggestions.push_back("FPS is below 60. Consider reducing graphics quality.");
    }
    
    if (getMemoryUsage() > 90.0f) {
        suggestions.push_back("High memory usage detected. Consider optimizing resource management.");
    }
    
    if (getCPUUsage() > 80.0f) {
        suggestions.push_back("High CPU usage detected. Consider optimizing background processes.");
    }
    
    return suggestions;
} 