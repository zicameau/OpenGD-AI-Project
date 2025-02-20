#include "LinuxInputManager.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <linux/joystick.h>

LinuxInputManager* LinuxInputManager::_instance = nullptr;

LinuxInputManager* LinuxInputManager::getInstance() {
    if (!_instance) {
        _instance = new LinuxInputManager();
    }
    return _instance;
}

bool LinuxInputManager::init() {
    _display = LinuxWindowManager::getInstance()->getDisplay();
    if (!_display) {
        std::cerr << "Failed to get X display for input manager" << std::endl;
        return false;
    }

    // Initialize default key mappings
    mapKey(XK_space, "jump");
    mapKey(XK_Return, "start");
    mapKey(XK_Escape, "pause");
    mapKey(XK_Left, "left");
    mapKey(XK_Right, "right");

    // Try to initialize default gamepad
    initGamepad("/dev/input/js0");

    return true;
}

void LinuxInputManager::update() {
    // Store previous key states
    _previousKeyState = _currentKeyState;

    // Update gamepad states
    updateGamepads();
}

void LinuxInputManager::handleKeyEvent(XEvent& event) {
    KeySym key = XkbKeycodeToKeysym(_display, event.xkey.keycode, 0, 0);
    
    if (event.type == KeyPress) {
        _currentKeyState[key] = true;
    } else if (event.type == KeyRelease) {
        _currentKeyState[key] = false;
    }
}

bool LinuxInputManager::initGamepad(const std::string& devicePath) {
    GamepadState gamepad;
    gamepad.devicePath = devicePath;
    
    int fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        gamepad.connected = false;
        return false;
    }
    
    close(fd);
    gamepad.connected = true;
    _gamepads.push_back(gamepad);
    return true;
}

void LinuxInputManager::updateGamepads() {
    for (auto& gamepad : _gamepads) {
        if (!gamepad.connected) continue;

        int fd = open(gamepad.devicePath.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            gamepad.connected = false;
            continue;
        }

        struct js_event event;
        while (read(fd, &event, sizeof(event)) > 0) {
            switch (event.type & ~JS_EVENT_INIT) {
                case JS_EVENT_BUTTON:
                    gamepad.buttons[event.number] = event.value;
                    break;
                case JS_EVENT_AXIS:
                    gamepad.axes[event.number] = event.value / 32767.0f;
                    break;
            }
        }
        
        close(fd);
    }
}

bool LinuxInputManager::isKeyPressed(KeySym key) const {
    auto it = _currentKeyState.find(key);
    return it != _currentKeyState.end() && it->second;
}

bool LinuxInputManager::isKeyJustPressed(KeySym key) const {
    return isKeyPressed(key) && !_previousKeyState.at(key);
}

bool LinuxInputManager::isKeyJustReleased(KeySym key) const {
    return !isKeyPressed(key) && _previousKeyState.at(key);
}

void LinuxInputManager::mapKey(KeySym key, std::string action) {
    _keyMappings[key] = action;
}

bool LinuxInputManager::isActionPressed(const std::string& action) const {
    for (const auto& mapping : _keyMappings) {
        if (mapping.second == action && isKeyPressed(mapping.first)) {
            return true;
        }
    }
    return false;
}

const LinuxInputManager::GamepadState& LinuxInputManager::getGamepadState(int gamepadId) const {
    if (gamepadId >= 0 && gamepadId < _gamepads.size()) {
        return _gamepads[gamepadId];
    }
    static GamepadState emptyState{false, "", {}, {}};
    return emptyState;
}

void LinuxInputManager::cleanup() {
    _currentKeyState.clear();
    _previousKeyState.clear();
    _keyMappings.clear();
    _gamepads.clear();
}

LinuxInputManager::~LinuxInputManager() {
    cleanup();
} 