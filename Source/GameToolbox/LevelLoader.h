#pragma once
#include <string>
#include "GJGameLevel.h"
#include "external/json.hpp"
#include "platform/FileUtils.h"
#include "GameToolbox/log.h"
#include <filesystem>

namespace LevelLoader {
    inline bool isXMLFormat(const std::string& content) {
        bool result = content.find("<k>") != std::string::npos && 
               (content.find("<k>kA2</k>") != std::string::npos || 
                content.find("<k>kA4</k>") != std::string::npos);
        GameToolbox::log("DEBUG: isXMLFormat check: {}", result);
        return result;
    }
    
    inline bool isJSONFormat(const std::string& content) {
        if (content.size() <= 2) {
            GameToolbox::log("DEBUG: isJSONFormat - content too short");
            return false;
        }
        
        bool result = (content.front() == '{' && content.back() == '}') ||
                     (content.front() == '[' && content.back() == ']');
        GameToolbox::log("DEBUG: isJSONFormat check: {}, first char: '{}', last char: '{}'", 
                        result, content.front(), content.back());
        return result;
    }
    
    inline bool isCompressedFormat(const std::string& content) {
        // Assume all level files are compressed
        GameToolbox::log("DEBUG: Assuming all levels are compressed");
        return true;
    }
    
    inline std::string getLevelString(GJGameLevel* level) {
        GameToolbox::log("=== GET LEVEL STRING START ===");
        if (!level) {
            GameToolbox::log("ERROR: Null level pointer");
            return "";
        }
        
        GameToolbox::log("DEBUG: Getting level string for level ID: {}", level->_levelID);
        
        if (!level->_levelString.empty()) {
            GameToolbox::log("DEBUG: Using cached level string, length: {}", level->_levelString.size());
            GameToolbox::log("=== GET LEVEL STRING END ===");
            return level->_levelString;
        }
        
        std::string levelID = std::to_string(level->_levelID);
        std::string levelStr;
        
        try {
            // First try to load from Custom/mainLevels.json
            auto fu = ax::FileUtils::getInstance();
            GameToolbox::log("DEBUG: Checking for Custom/mainLevels.json");
            if (fu->isFileExist("Custom/mainLevels.json")) {
                GameToolbox::log("DEBUG: mainLevels.json exists, loading content");
                std::string content = fu->getStringFromFile("Custom/mainLevels.json");
                if (!content.empty()) {
                    try {
                        GameToolbox::log("DEBUG: Parsing mainLevels.json");
                        nlohmann::json file = nlohmann::json::parse(content);
                        GameToolbox::log("DEBUG: Checking if level {} exists in mainLevels.json", levelID);
                        if (file.contains(levelID)) {
                            GameToolbox::log("DEBUG: Level {} found in mainLevels.json", levelID);
                            std::string levelData = file.at(levelID).get<std::string>();
                            if (!levelData.empty()) {
                                GameToolbox::log("SUCCESS: Loaded level {} from mainLevels.json, size: {}", 
                                                levelID, levelData.size());
                                level->_levelString = levelData;
                                GameToolbox::log("=== GET LEVEL STRING END ===");
                                return levelData;
                            } else {
                                GameToolbox::log("DEBUG: Level data is empty in mainLevels.json");
                            }
                        } else {
                            GameToolbox::log("DEBUG: Level {} not found in mainLevels.json", levelID);
                        }
                    } catch (const std::exception& e) {
                        GameToolbox::log("ERROR: Parsing mainLevels.json: {}", e.what());
                    }
                } else {
                    GameToolbox::log("DEBUG: mainLevels.json is empty");
                }
            } else {
                GameToolbox::log("DEBUG: mainLevels.json not found");
            }
            
            // If not found, try to load directly from levels directory
            std::string levelPath = fmt::format("levels/{}.txt", levelID);
            GameToolbox::log("DEBUG: Trying to load level from {}", levelPath);
            if (fu->isFileExist(levelPath)) {
                GameToolbox::log("DEBUG: Level file exists, loading content");
                std::string content = fu->getStringFromFile(levelPath);
                if (!content.empty()) {
                    GameToolbox::log("SUCCESS: Loaded level {} from file, size: {}", levelID, content.size());
                    GameToolbox::log("First 50 chars: {}", content.substr(0, 50));
                    level->_levelString = content;
                    GameToolbox::log("=== GET LEVEL STRING END ===");
                    return content;
                } else {
                    GameToolbox::log("ERROR: Level file {} is empty", levelPath);
                }
            } else {
                GameToolbox::log("ERROR: Level file {} not found", levelPath);
            }
        } catch (const std::exception& e) {
            GameToolbox::log("EXCEPTION loading level {}: {}", levelID, e.what());
        }
        
        GameToolbox::log("ERROR: Failed to load level {}", levelID);
        GameToolbox::log("=== GET LEVEL STRING END ===");
        return "";
    }
} 