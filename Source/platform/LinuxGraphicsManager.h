#pragma once
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class LinuxGraphicsManager {
public:
    static LinuxGraphicsManager* getInstance() {
        static std::once_flag flag;
        static LinuxGraphicsManager* instance = nullptr;
        
        std::call_once(flag, []() {
            instance = new LinuxGraphicsManager();
        });
        
        return instance;
    }

    bool initializeContext();
    GLuint compileShader(const std::string& source, GLenum type);
    bool loadTexture(const std::string& path);
    void cleanup();

    virtual ~LinuxGraphicsManager();
    
    // Delete copy and move operations
    LinuxGraphicsManager(const LinuxGraphicsManager&) = delete;
    LinuxGraphicsManager& operator=(const LinuxGraphicsManager&) = delete;
    LinuxGraphicsManager(LinuxGraphicsManager&&) = delete;
    LinuxGraphicsManager& operator=(LinuxGraphicsManager&&) = delete;

private:
    LinuxGraphicsManager() = default;
    std::mutex _contextMutex;
    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> _window;
    std::vector<GLuint> _shaders;
    std::vector<GLuint> _textures;
}; 