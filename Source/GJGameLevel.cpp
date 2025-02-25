/*************************************************************************
    OpenGD - Open source Geometry Dash.
    Copyright (C) 2023  OpenGD Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License    
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*************************************************************************/

#include "GJGameLevel.h"
#include "GameToolbox/conv.h"
#include "GameToolbox/log.h"

#include "external/base64.h"

#include <ZipUtils.h>
#include <algorithm>
#include <string>
#include <vector>

// Updated decompression function using existing functionality
std::string GJGameLevel::decompressLvlStr(const std::string& str) {
	GameToolbox::log("=== DECOMPRESSION START ===");
	if (str.empty()) {
		GameToolbox::log("ERROR: Cannot decompress empty string");
		return "";
	}
	
	GameToolbox::log("Input string length: {}", str.size());
	GameToolbox::log("First 20 chars: {}", str.substr(0, 20));
	
	try {
		// Check if the string is already in the correct format for decompression
		std::string input = str;
		
		// Check if it starts with H4sI (base64 encoded gzip data)
		if (input.substr(0, 4) == "H4sI") {
			GameToolbox::log("DEBUG: Detected base64 gzip header");
			
			// Decode base64 using the existing function
			GameToolbox::log("DEBUG: Decoding base64...");
			std::string decoded = base64_decode(input);
			if (decoded.empty()) {
				GameToolbox::log("ERROR: Base64 decoding failed!");
				return "";
			}
			
			GameToolbox::log("DEBUG: Base64 decode successful, decoded length: {}", decoded.size());
			
			// Use a more conservative memory hint for decompression
			// Start with a smaller multiplier and retry with larger if needed
			unsigned char* inflated = nullptr;
			ssize_t inflatedLen = 0;
			
			// Try with progressively larger output buffer sizes
			for (int multiplier = 2; multiplier <= 20; multiplier += 2) {
				GameToolbox::log("DEBUG: Trying decompression with multiplier {}", multiplier);
				
				// Free previous attempt if any
				if (inflated) {
					free(inflated);
					inflated = nullptr;
				}
				
				// Try to decompress
				GameToolbox::log("DEBUG: Calling inflateMemoryWithHint...");
				inflatedLen = ax::ZipUtils::inflateMemoryWithHint(
					(unsigned char*)decoded.data(), 
					decoded.size(), 
					&inflated, 
					decoded.size() * multiplier
				);
				
				GameToolbox::log("DEBUG: After inflateMemoryWithHint call");
				
				// If successful, break out of the loop
				if (inflated && inflatedLen > 0) {
					GameToolbox::log("DEBUG: Decompression successful with multiplier {}", multiplier);
					break;
				}
				
				GameToolbox::log("DEBUG: Decompression failed with multiplier {}, trying larger buffer", multiplier);
			}
			
			// Check if any of the attempts succeeded
			if (!inflated || inflatedLen <= 0) {
				GameToolbox::log("ERROR: All decompression attempts failed");
				return "";
			}
			
			GameToolbox::log("DEBUG: Decompression successful, inflatedLen: {}", inflatedLen);
			
			// Convert to string
			std::string result(reinterpret_cast<char*>(inflated), inflatedLen);
			GameToolbox::log("DEBUG: Converted to string, length: {}", result.size());
			GameToolbox::log("First 50 chars of result: {}", result.substr(0, 50));
			
			free(inflated); // Free the inflated buffer
			GameToolbox::log("DEBUG: Freed inflated buffer");
			
			GameToolbox::log("=== DECOMPRESSION END ===");
			return result;
		} else {
			GameToolbox::log("DEBUG: Input is not in expected compressed format");
			GameToolbox::log("=== DECOMPRESSION END ===");
			return str; // Return the original string if it's not in the expected format
		}
	} catch (const std::exception& e) {
		GameToolbox::log("EXCEPTION during decompression: {}", e.what());
		GameToolbox::log("=== DECOMPRESSION END ===");
		return "";
	}
}

GJGameLevel* GJGameLevel::create()
{
	auto ret = new GJGameLevel();
	return ret;
}

GJGameLevel* GJGameLevel::createWithResponse(std::string_view backendResponse)
{
	auto ret = new GJGameLevel();

	std::string_view::size_type pos = 0;
	std::string_view::size_type nextPos = 0;

	// Iterate through the response string
	while ((nextPos = backendResponse.find(':', pos)) != std::string_view::npos)
	{
		std::string_view key = backendResponse.substr(pos, nextPos - pos);
		pos = nextPos + 1;

		nextPos = backendResponse.find(':', pos);
		if (nextPos == std::string_view::npos)
			break;

		std::string_view value = backendResponse.substr(pos, nextPos - pos);
		pos = nextPos + 1;

		// Process key-value pairs
		if (key == "1") ret->_levelID = GameToolbox::stoi(value);
		else if (key == "2") ret->_levelName = value;
		else if (key == "3") ret->_description = value;
		else if (key == "4") ret->_levelString = value;
		else if (key == "5") ret->_levelCreator = value;
		else if (key == "6") ret->_difficultyDenominator = GameToolbox::stoi(value);
		else if (key == "7") ret->_difficultyNumerator = GameToolbox::stoi(value);
		else if (key == "8") ret->_downloads = GameToolbox::stoi(value);
		else if (key == "9") ret->_officialSongID = GameToolbox::stoi(value);
		else if (key == "10") ret->_gameVersion = GameToolbox::stoi(value);
		else if (key == "11") ret->_likes = GameToolbox::stoi(value);
		else if (key == "12") ret->_length = GameToolbox::stoi(value);
		else if (key == "13") ret->_demon = GameToolbox::stoi(value) == 1;
		else if (key == "14") ret->_stars = GameToolbox::stoi(value);
		else if (key == "15") ret->_featureScore = GameToolbox::stoi(value);
		else if (key == "16") ret->_auto = GameToolbox::stoi(value) == 1;
		else if (key == "17") ret->_XORPassword = value;
		else if (key == "18") ret->_uploadDate = value;
		else if (key == "19") ret->_updateDate = value;
		else if (key == "27") ret->_verifiedCoins = GameToolbox::stoi(value) == 1;
		else if (key == "35") ret->_songName = value;
		else if (key == "36") ret->_songID = GameToolbox::stoi(value);
		else if (key == "37") ret->_sondURL = value;
		else if (key == "38") ret->_gameVersion = GameToolbox::stoi(value);
		else if (key == "39") ret->_gameVersion = GameToolbox::stoi(value);
		else if (key == "41") ret->_normalPercent = GameToolbox::stof(value);
		else if (key == "42") ret->_practicePercent = GameToolbox::stof(value);
		else if (key == "43") ret->_likes = GameToolbox::stoi(value);
		else if (key == "45") ret->_dislikes = GameToolbox::stoi(value);
		else if (key == "46") ret->_stars = GameToolbox::stoi(value);
		else if (key == "47") ret->_starsRequested = GameToolbox::stoi(value) == 1;
	}

	return ret;
}

GJGameLevel* GJGameLevel::createWithMinimumData(std::string levelName, std::string levelCreator, int levelID)
{
	auto level = GJGameLevel::create();

	level->_levelName = levelName;
	level->_levelID = levelID;
	level->_levelCreator = levelCreator;

	return level;
}

std::string GJGameLevel::getDifficultySprite(GJGameLevel* level, DifficultyType type)
{
	if (level->_auto)
	{
		return "difficulty_auto_btn_001.png";
	}

	int diff = 0;

	if (level->_difficultyNumerator != 0) diff = level->_difficultyNumerator / 10;

	if (level->_demon)
	{
		switch (level->_demonDifficulty) {
		case 3:
			return type == kLevelInfoLayer ? "difficulty_07_btn2_001.png" : "difficulty_07_btn_001.png";
		case 4:
			return type == kLevelInfoLayer ? "difficulty_08_btn2_001.png" : "difficulty_08_btn_001.png";
		case 5:
			return type == kLevelInfoLayer ? "difficulty_09_btn2_001.png" : "difficulty_09_btn_001.png";
		case 6:
			return type == kLevelInfoLayer ? "difficulty_10_btn2_001.png" : "difficulty_10_btn_001.png";
		default:
		case 0:
			return type == kLevelInfoLayer ? "difficulty_06_btn2_001.png" : "difficulty_06_btn_001.png";
		}
	}

	switch (diff)
	{
	case 1:
		return type == kMainLevels ? "diffIcon_01_btn_001.png" : "difficulty_01_btn_001.png";
	case 2:
		return type == kMainLevels ? "diffIcon_02_btn_001.png" : "difficulty_02_btn_001.png";
	case 3:
		return type == kMainLevels ? "diffIcon_03_btn_001.png" : "difficulty_03_btn_001.png";
	case 4:
		return type == kMainLevels ? "diffIcon_04_btn_001.png" : "difficulty_04_btn_001.png";
	case 5:
		return type == kMainLevels ? "diffIcon_05_btn_001.png" : "difficulty_05_btn_001.png";
	case 6:
		return type == kMainLevels ? "diffIcon_06_btn_001.png" : "difficulty_06_btn_001.png";
	default:
		return type == kMainLevels ? "diffIcon_01_btn_001.png" : "difficulty_00_btn_001.png";
	}
}