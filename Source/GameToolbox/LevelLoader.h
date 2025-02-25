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
                    nlohmann::json file = nlohmann::json::parse(content);
                    if (file.contains(levelID)) {
                        levelStr = fmt::format("H4sIAAAAAAAAA{}", file.at(levelID).get<std::string>());
                        return levelStr;
                    }
                }
            }
            
            // If not found, try to load directly from levels directory
            std::string levelPath = fmt::format("levels/{}.txt", levelID);
            if (fu->isFileExist(levelPath)) {
                std::string content = fu->getStringFromFile(levelPath);
                if (!content.empty()) {
                    // Check if it already has the H4sIAAAAAAAAA prefix
                    if (content.length() > 14 && content.substr(0, 14) == "H4sIAAAAAAAAA") {
                        return content;
                    } else {
                        return fmt::format("H4sIAAAAAAAAA{}", content);
                    }
                }
            }
        } catch (const std::exception& e) {
            GameToolbox::log("Error loading level {}: {}", levelID, e.what());
        }
        
        return levelStr;
    }
} 