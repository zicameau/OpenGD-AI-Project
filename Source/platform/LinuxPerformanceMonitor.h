#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>

struct PerformanceMetrics {
    float fps;
    float frameTime;
    float cpuUsage;
    float memoryUsage;
    float gpuUsage;
    float diskIO;
    float networkLatency;
};

class LinuxPerformanceMonitor {
public:
    static LinuxPerformanceMonitor* getInstance();

    // FPS and Frame Time
    void beginFrame();
    void endFrame();
    float getCurrentFPS() const;
    float getAverageFrameTime() const;
    
    // Resource Monitoring
    float getCPUUsage() const;
    float getMemoryUsage() const;
    float getGPUUsage() const;
    
    // Profiling
    void beginProfile(const std::string& name);
    void endProfile(const std::string& name);
    std::unordered_map<std::string, float> getProfilingResults() const;
    
    // Performance Logging
    void enableLogging(bool enable);
    void setLogInterval(float seconds);
    std::vector<PerformanceMetrics> getPerformanceHistory() const;
    
    // Optimization Suggestions
    std::vector<std::string> getOptimizationSuggestions() const;

private:
    LinuxPerformanceMonitor() = default;
    
    std::chrono::high_resolution_clock::time_point _frameStart;
    std::vector<float> _fpsHistory;
    std::vector<float> _frameTimeHistory;
    std::vector<PerformanceMetrics> _metricsHistory;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> _profileStartTimes;
    std::unordered_map<std::string, float> _profilingResults;
    bool _loggingEnabled = false;
    float _logInterval = 1.0f;
}; 