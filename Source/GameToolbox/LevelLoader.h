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
                                // Check if it already has the H4sIAAAAAAAAA prefix
                                if (levelData.length() > 14 && levelData.substr(0, 14) == "H4sIAAAAAAAAA") {
                                    levelStr = levelData;
                                } else {
                                    levelStr = fmt::format("H4sIAAAAAAAAA{}", levelData);
                                }
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
                    
                    // Try to determine if the content is already compressed
                    bool isCompressed = false;
                    
                    // Check if it already has the H4sIAAAAAAAAA prefix
                    if (content.length() > 14 && content.substr(0, 14) == "H4sIAAAAAAAAA") {
                        isCompressed = true;
                        levelStr = content;
                    } else {
                        // Try to detect if it's a raw level string (XML-like format)
                        if (content.find("<k>kA2</k>") != std::string::npos || 
                            content.find("<k>kA4</k>") != std::string::npos) {
                            // This appears to be an uncompressed level string
                            GameToolbox::log("Level {} appears to be uncompressed XML", levelID);
                            level->_levelString = content;
                            return content;
                        } else {
                            // Assume it's base64 data that needs the prefix
                            levelStr = fmt::format("H4sIAAAAAAAAA{}", content);
                            isCompressed = true;
                        }
                    }
                    
                    if (isCompressed) {
                        // Test decompression before returning
                        std::string decompressed = GJGameLevel::decompressLvlStr(levelStr);
                        if (!decompressed.empty()) {
                            GameToolbox::log("Successfully decompressed level {}", levelID);
                            level->_levelString = levelStr;
                            return levelStr;
                        } else {
                            GameToolbox::log("Failed to decompress level {}, using raw content", levelID);
                            // If decompression failed, just use the raw content
                            level->_levelString = content;
                            return content;
                        }
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
        return "";
    }
} 