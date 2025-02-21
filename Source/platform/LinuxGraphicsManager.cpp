#include <vector>
#include <string>
#include "LinuxGraphicsManager.h"
#include <stdexcept>
#include <syslog.h>

// Add shader compatibility testing
bool LinuxGraphicsManager::testShaderCompatibility() {
    // Test critical shaders
    std::vector<std::string> shaders = {
        "basic.vert", "basic.frag",
        "effects.vert", "effects.frag"
    };
    
    for (const auto& shader : shaders) {
        if (!validateShader(shader)) {
            log(LogLevel::ERROR, "Shader compatibility test failed: " + shader);
            return false;
        }
    }
    return true;
}

// Add texture loading verification
bool LinuxGraphicsManager::verifyTextureLoading() {
    // Test various texture formats
    std::vector<std::string> textures = {
        "test_rgba.png", "test_rgb.png",
        "test_compressed.png"
    };
    
    for (const auto& texture : textures) {
        if (!validateTexture(texture)) {
            log(LogLevel::ERROR, "Texture loading test failed: " + texture);
            return false;
        }
    }
    return true;
}

bool LinuxGraphicsManager::initializeContext() {
    std::lock_guard<std::mutex> lock(_contextMutex);
    try {
        if (!glfwInit()) {
            syslog(LOG_ERR, "Failed to initialize GLFW");
            return false;
        }
        
        // Set error callback
        glfwSetErrorCallback([](int error, const char* description) {
            syslog(LOG_ERR, "GLFW Error (%d): %s", error, description);
        });
        
        // Create window with RAII
        _window = std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>(
            glfwCreateWindow(800, 600, "OpenGD", nullptr, nullptr),
            [](GLFWwindow* w) { 
                if (w) glfwDestroyWindow(w); 
                glfwTerminate();
            }
        );
        
        if (!_window) {
            throw std::runtime_error("Failed to create OpenGL context");
        }
        
        glfwMakeContextCurrent(_window.get());
        
        if (glewInit() != GLEW_OK) {
            throw std::runtime_error("Failed to initialize GLEW");
        }
        
        return true;
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Graphics initialization failed: %s", e.what());
        cleanup();
        return false;
    }
}

GLuint LinuxGraphicsManager::compileShader(const std::string& source, GLenum type) {
    try {
        GLuint shader = glCreateShader(type);
        if (!shader) {
            throw std::runtime_error("Failed to create shader");
        }
        
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            glDeleteShader(shader);
            throw std::runtime_error(std::string("Shader compilation failed: ") + infoLog);
        }
        
        _shaders.push_back(shader);
        return shader;
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Shader compilation failed: %s", e.what());
        return 0;
    }
}

bool LinuxGraphicsManager::loadTexture(const std::string& path) {
    try {
        GLuint texture;
        glGenTextures(1, &texture);
        if (!texture) {
            throw std::runtime_error("Failed to generate texture");
        }
        
        // Use RAII for texture cleanup on failure
        struct TextureGuard {
            GLuint id;
            bool committed = false;
            ~TextureGuard() {
                if (!committed && id) glDeleteTextures(1, &id);
            }
        } guard{texture};
        
        // Load and validate texture data
        // ... texture loading implementation ...
        
        guard.committed = true;
        _textures.push_back(texture);
        return true;
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Texture loading failed: %s", e.what());
        return false;
    }
}

void LinuxGraphicsManager::cleanup() {
    std::lock_guard<std::mutex> lock(_contextMutex);
    
    // Cleanup shaders
    for (GLuint shader : _shaders) {
        glDeleteShader(shader);
    }
    _shaders.clear();
    
    // Cleanup textures
    for (GLuint texture : _textures) {
        glDeleteTextures(1, &texture);
    }
    _textures.clear();
    
    // Window cleanup handled by unique_ptr
    _window.reset();
}

LinuxGraphicsManager::~LinuxGraphicsManager() {
    cleanup();
} 
