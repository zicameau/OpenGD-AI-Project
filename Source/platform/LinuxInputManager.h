#pragma once
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <linux/input.h>
#include <vector>
#include <map>
#include <string>
#include "LinuxWindowManager.h"

class LinuxInputManager {
public:
    static LinuxInputManager* getInstance();
    
    bool init();
    void update();
    void cleanup();

    // Keyboard state
    bool isKeyPressed(KeySym key) const;
    bool isKeyJustPressed(KeySym key) const;
    bool isKeyJustReleased(KeySym key) const;

    // Gamepad support
    struct GamepadState {
        bool connected;
        std::string devicePath;
        std::map<int, bool> buttons;
        std::map<int, float> axes;
    };

    bool initGamepad(const std::string& devicePath);
    const GamepadState& getGamepadState(int gamepadId) const;
    
    // Input mapping
    void mapKey(KeySym key, std::string action);
    bool isActionPressed(const std::string& action) const;

private:
    LinuxInputManager() = default;
    ~LinuxInputManager();

    void handleKeyEvent(XEvent& event);
    void updateGamepads();
    
    Display* _display = nullptr;
    std::map<KeySym, bool> _currentKeyState;
    std::map<KeySym, bool> _previousKeyState;
    std::map<KeySym, std::string> _keyMappings;
    std::vector<GamepadState> _gamepads;
    
    static LinuxInputManager* _instance;
}; 