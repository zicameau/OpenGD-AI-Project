#include "LinuxWindowManager.h"
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <stdexcept>
#include <iostream>

LinuxWindowManager* LinuxWindowManager::_instance = nullptr;

LinuxWindowManager* LinuxWindowManager::getInstance() {
    if (!_instance) {
        _instance = new LinuxWindowManager();
    }
    return _instance;
}

bool LinuxWindowManager::init(int width, int height, const std::string& title) {
    _display = XOpenDisplay(nullptr);
    if (!_display) {
        std::cerr << "Failed to open X display" << std::endl;
        return false;
    }

    // Check GLX version
    int glxMajor, glxMinor;
    if (!glXQueryVersion(_display, &glxMajor, &glxMinor)) {
        std::cerr << "Failed to query GLX version" << std::endl;
        return false;
    }

    if ((glxMajor <= 1) && (glxMinor < 3)) {
        std::cerr << "GLX 1.3 or higher required" << std::endl;
        return false;
    }

    // Configure OpenGL framebuffer
    static int visualAttribs[] = {
        GLX_RGBA,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER,
        None
    };

    XVisualInfo* vi = glXChooseVisual(_display, DefaultScreen(_display), visualAttribs);
    if (!vi) {
        std::cerr << "Failed to create visual info" << std::endl;
        return false;
    }

    // Create window
    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap(_display, RootWindow(_display, vi->screen),
                                  vi->visual, AllocNone);
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ButtonReleaseMask | StructureNotifyMask;

    _window = XCreateWindow(_display, RootWindow(_display, vi->screen),
                           0, 0, width, height, 0, vi->depth, InputOutput,
                           vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);

    if (!_window) {
        std::cerr << "Failed to create window" << std::endl;
        XFree(vi);
        return false;
    }

    // Set window title
    XStoreName(_display, _window, title.c_str());

    // Create GL context
    _glContext = glXCreateContext(_display, vi, nullptr, GL_TRUE);
    XFree(vi);

    if (!_glContext) {
        std::cerr << "Failed to create GL context" << std::endl;
        return false;
    }

    // Handle window close
    _wmDeleteWindow = XInternAtom(_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(_display, _window, &_wmDeleteWindow, 1);

    // Show window
    XMapWindow(_display, _window);
    glXMakeCurrent(_display, _window, _glContext);

    return true;
}

void LinuxWindowManager::cleanup() {
    if (_glContext) {
        glXMakeCurrent(_display, None, nullptr);
        glXDestroyContext(_display, _glContext);
        _glContext = nullptr;
    }

    if (_window) {
        XDestroyWindow(_display, _window);
        _window = 0;
    }

    if (_display) {
        XCloseDisplay(_display);
        _display = nullptr;
    }
}

void LinuxWindowManager::processEvents() {
    XEvent event;
    while (XPending(_display)) {
        XNextEvent(_display, &event);
        
        switch (event.type) {
            case ClientMessage:
                if (event.xclient.data.l[0] == _wmDeleteWindow) {
                    _shouldClose = true;
                }
                break;
                
            case ConfigureNotify:
                // Handle window resize
                break;
                
            case KeyPress:
            case KeyRelease:
                // Handle keyboard input
                break;
                
            case ButtonPress:
            case ButtonRelease:
                // Handle mouse input
                break;
        }
    }
}

void LinuxWindowManager::swapBuffers() {
    glXSwapBuffers(_display, _window);
}

bool LinuxWindowManager::shouldClose() const {
    return _shouldClose;
}

LinuxWindowManager::~LinuxWindowManager() {
    cleanup();
}

void LinuxWindowManager::setTitle(const std::string& title) {
    XStoreName(_display, _window, title.c_str());
}

void LinuxWindowManager::setSize(int width, int height) {
    XResizeWindow(_display, _window, width, height);
}

void LinuxWindowManager::getSize(int& width, int& height) const {
    XWindowAttributes attributes;
    XGetWindowAttributes(_display, _window, &attributes);
    width = attributes.width;
    height = attributes.height;
} 