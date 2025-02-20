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

#pragma once

#include <string>
#include <string_view>

/**
 * @namespace LevelTools
 * @brief Utilities for level management and verification
 * 
 * Provides functionality for:
 * - Level data validation
 * - Level string processing
 * - Audio management
 */
namespace LevelTools {
	/**
	 * @brief Verifies the integrity of level data
	 * @param levelData Raw level data to verify
	 * @param id Level ID
	 * @return bool True if level data is valid
	 */
	bool verifyLevelIntegrity(std::string_view levelData, int id);
	
	/**
	 * @brief Gets the filename for level audio
	 * @param lid Level ID
	 * @return std::string Audio filename
	 */
	std::string getAudioFilename(int lid);

	/**
	 * @brief Gets the title of level audio
	 * @param lid Level ID
	 * @return std::string Audio title
	 */
	std::string getAudioTitle(int lid);

	int getArtistForAudio(int lid);
	std::string getNameForArtist(int artistId);
	std::string getFbURLForArtist(int artistId);
	std::string getNgURLForArtist(int artistId);
	std::string getYtURLForArtist(int artistId);
	std::string getURLForAudio(int lid);
}
