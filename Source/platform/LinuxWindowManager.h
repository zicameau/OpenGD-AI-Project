#pragma once
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <string>
#include <memory>

class LinuxWindowManager {
public:
    static LinuxWindowManager* getInstance();
    bool init(int width, int height, const std::string& title);
    void cleanup();
    
    void swapBuffers();
    void processEvents();
    bool shouldClose() const;
    
    // Window properties
    void setTitle(const std::string& title);
    void setSize(int width, int height);
    void getSize(int& width, int& height) const;
    
    // X11 specific
    Display* getDisplay() const { return _display; }
    Window getWindow() const { return _window; }
    GLXContext getGLContext() const { return _glContext; }

private:
    LinuxWindowManager() = default;
    ~LinuxWindowManager();
    
    bool createWindow(int width, int height, const std::string& title);
    bool initializeGL();
    
    Display* _display = nullptr;
    Window _window = 0;
    GLXContext _glContext = nullptr;
    Atom _wmDeleteWindow = 0;
    bool _shouldClose = false;
    
    static LinuxWindowManager* _instance;
}; 