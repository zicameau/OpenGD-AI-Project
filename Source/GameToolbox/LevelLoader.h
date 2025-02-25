#pragma once
#include <string>
#include "GJGameLevel.h"
#include "external/json.hpp"
#include "platform/FileUtils.h"
#include "GameToolbox/log.h"
#include <filesystem>

namespace LevelLoader {
    inline std::string getLevelString(GJGameLevel* level) {
        if (!level->_levelString.empty()) {
            return level->_levelString;
        }
        
        std::string levelID = std::to_string(level->_levelID);
        std::string levelStr;
        
        try {
            // First try to load from Custom/mainLevels.json
            auto fu = ax::FileUtils::getInstance();
            if (fu->isFileExist("Custom/mainLevels.json")) {
                std::string content = fu->getStringFromFile("Custom/mainLevels.json");
                if (!content.empty()) {
                    try {
                        nlohmann::json file = nlohmann::json::parse(content);
                        if (file.contains(levelID)) {
                            std::string levelData = file.at(levelID).get<std::string>();
                            if (!levelData.empty()) {
                                levelStr = fmt::format("H4sIAAAAAAAAA{}", levelData);
                                GameToolbox::log("Loaded level {} from mainLevels.json", levelID);
                                return levelStr;
                            }
                        }
                    } catch (const std::exception& e) {
                        GameToolbox::log("Error parsing mainLevels.json: {}", e.what());
                    }
                }
            }
            
            // If not found, try to load directly from levels directory
            std::string levelPath = fmt::format("levels/{}.txt", levelID);
            GameToolbox::log("Trying to load level from {}", levelPath);
            if (fu->isFileExist(levelPath)) {
                std::string content = fu->getStringFromFile(levelPath);
                if (!content.empty()) {
                    GameToolbox::log("Loaded level {} from file, size: {}", levelID, content.size());
                    // Check if it already has the H4sIAAAAAAAAA prefix
                    if (content.length() > 14 && content.substr(0, 14) == "H4sIAAAAAAAAA") {
                        return content;
                    } else {
                        return fmt::format("H4sIAAAAAAAAA{}", content);
                    }
                } else {
                    GameToolbox::log("Level file {} is empty", levelPath);
                }
            } else {
                GameToolbox::log("Level file {} not found", levelPath);
            }
        } catch (const std::exception& e) {
            GameToolbox::log("Error loading level {}: {}", levelID, e.what());
        }
        
        GameToolbox::log("Failed to load level {}", levelID);
        return levelStr;
    }
} 