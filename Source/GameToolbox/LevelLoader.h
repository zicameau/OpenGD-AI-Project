#pragma once
#include <string>
#include "GJGameLevel.h"
#include "external/json.hpp"
#include "platform/FileUtils.h"
#include "GameToolbox/log.h"
#include <filesystem>

namespace LevelLoader {
    inline bool isXMLFormat(const std::string& content) {
        return content.find("<k>") != std::string::npos && 
               (content.find("<k>kA2</k>") != std::string::npos || 
                content.find("<k>kA4</k>") != std::string::npos);
    }
    
    inline bool isJSONFormat(const std::string& content) {
        return (content.front() == '{' && content.back() == '}') ||
               (content.front() == '[' && content.back() == ']');
    }
    
    inline bool isCompressedFormat(const std::string& content) {
        return content.length() > 14 && content.substr(0, 14) == "H4sIAAAAAAAAA";
    }
    
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
                                GameToolbox::log("Loaded level {} from mainLevels.json", levelID);
                                
                                // Determine format and handle accordingly
                                if (isCompressedFormat(levelData)) {
                                    level->_levelString = levelData;
                                    return levelData;
                                } else if (isXMLFormat(levelData) || isJSONFormat(levelData)) {
                                    level->_levelString = levelData;
                                    return levelData;
                                } else {
                                    // Assume it needs compression prefix
                                    levelStr = fmt::format("H4sIAAAAAAAAA{}", levelData);
                                    level->_levelString = levelStr;
                                    return levelStr;
                                }
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
                    
                    // Determine format and handle accordingly
                    if (isCompressedFormat(content)) {
                        GameToolbox::log("Level {} is in compressed format", levelID);
                        std::string decompressed = GJGameLevel::decompressLvlStr(content);
                        if (!decompressed.empty()) {
                            GameToolbox::log("Successfully decompressed level {}", levelID);
                            level->_levelString = content;
                            return content;
                        } else {
                            GameToolbox::log("Failed to decompress level {}, using raw content", levelID);
                            level->_levelString = content;
                            return content;
                        }
                    } else if (isXMLFormat(content)) {
                        GameToolbox::log("Level {} is in XML format", levelID);
                        level->_levelString = content;
                        return content;
                    } else if (isJSONFormat(content)) {
                        GameToolbox::log("Level {} is in JSON format", levelID);
                        level->_levelString = content;
                        return content;
                    } else {
                        // Unknown format, try adding compression prefix
                        GameToolbox::log("Level {} is in unknown format, trying with compression prefix", levelID);
                        levelStr = fmt::format("H4sIAAAAAAAAA{}", content);
                        std::string decompressed = GJGameLevel::decompressLvlStr(levelStr);
                        if (!decompressed.empty()) {
                            GameToolbox::log("Successfully processed level {} with compression prefix", levelID);
                            level->_levelString = levelStr;
                            return levelStr;
                        } else {
                            GameToolbox::log("Failed to process level {}, using raw content", levelID);
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