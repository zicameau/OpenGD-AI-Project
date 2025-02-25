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

#include "GameManager.h"
#include "ResourcesLoadingLayer.h"
#include "LoadingLayer.h"
#include "Config.h"
#include "external/json.hpp"
#include "GameToolbox/log.h"
#include "GameToolbox/getTextureString.h"

#include <2d/Scene.h>
#include <2d/Label.h>
#include <2d/ActionInterval.h>
#include <2d/ActionInstant.h>
#include <platform/FileUtils.h>
#include <base/Director.h>

USING_NS_AX;

ResourcesLoadingLayer* ResourcesLoadingLayer::create() {
	ResourcesLoadingLayer* pRet = new ResourcesLoadingLayer();
	if (pRet->init()) {
		pRet->autorelease();
		return pRet;
	} else {
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

Scene* ResourcesLoadingLayer::scene()
{
	auto scene = ax::Scene::create();
	scene->addChild(ResourcesLoadingLayer::create());
	return scene;
}

void ResourcesLoadingLayer::loadLoadingLayer()
{
	this->runAction(Sequence::create(DelayTime::create(0), CallFunc::create([]() {
		Director::getInstance()->replaceScene(LoadingLayer::scene()); 
	}), nullptr));
}

bool ResourcesLoadingLayer::init()
{
	_fu = FileUtils::getInstance();

	for (std::string_view a : _fu->getSearchPaths())
	{
		GameToolbox::log("search path: {}", a);
	}
	//some random files to check if theres already a path added
	if (_fu->isFileExist("game_bg_01_001-hd.png") && _fu->isFileExist("GJ_LaunchSheet-hd.png") && _fu->isFileExist("GJ_GameSheet03-uhd.png"))
	{
		loadLoadingLayer();
		return true;
	}

	_gm = GameManager::getInstance();
	_dir = Director::getInstance();
	_writablePath = _fu->getWritablePath();

	auto winSize = _dir->getWinSize();
	_posMiddle = { winSize.width / 2, winSize.height / 2 };

	if(auto path = _gm->get<std::string>("resources_path"); !path.empty() && _fu->isDirectoryExist(path))
	{
		_fu->addSearchPath(path, true);
		loadLoadingLayer();
		return true;
	}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
	handleWindows();
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
	handleMac();
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
	handleLinux();
#endif
	return true;
}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>


void ResourcesLoadingLayer::handleWindows()
{
	auto label = Label::create();
	label->setString("Geometry Dash Resources not found!\n Waiting for GeometryDash.exe");
	label->setSystemFontSize(20);
	label->setPosition(_posMiddle);
	addChild(label);

	_gdProcessAction = RepeatForever::create(Sequence::create(DelayTime::create(0.6), CallFunc::create([=]() {
		static int nPoints = -1;
		++nPoints;
		if (nPoints > 3)
			nPoints = 0;

		label->setString(fmt::format("Geometry Dash Resources not found!\n Waiting for GeometryDash.exe{}", nPoints == 1 ? "." : nPoints == 2 ? ".." : nPoints == 3 ? "..." : ""));

		if (std::string exepath = ResourcesLoadingLayer::getRunningGDPathWindows(); !exepath.empty())
		{
			if (isWindowsGDPathValid(exepath))
			{
				exepath.erase(exepath.find_last_of('\\'));
				std::string resourcesPath = fmt::format("{}\\Resources", exepath);
				_fu->addSearchPath(resourcesPath, true);
				_gm->set<std::string>("resources_path", resourcesPath);
				_gm->save();
				loadLoadingLayer();
			}
			else
			{
				label->setString("Invalid GeometryDash.exe path found...");
				nPoints = 0;
			}
		}
	}), nullptr));

	this->runAction(_gdProcessAction);
}

bool ResourcesLoadingLayer::isWindowsGDPathValid(std::string exepath)
{
	exepath.erase(exepath.find_last_of('\\'));

	if (!_fu->isDirectoryExist(fmt::format("{}\\Resources", exepath))) return false;
	if (!_fu->isFileExist(fmt::format("{}\\libcocos2d.dll", exepath))) return false;
	if (!_fu->isFileExist(fmt::format("{}\\libcurl.dll", exepath))) return false;

	return true;
}


std::string ResourcesLoadingLayer::getRunningGDPathWindows()
{
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return "";
	}

	cProcesses = cbNeeded / sizeof(DWORD);

	for (unsigned int i = 0; i < cProcesses; i++)
	{
		if (DWORD processID = aProcesses[i]; processID != 0)
		{
			HANDLE processHandle = NULL;
			TCHAR filename[MAX_PATH];
			processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
			if (processHandle != NULL)
			{
				if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) != 0)
				{
					std::wstring str(filename);
					if (str.find(L"GeometryDash.exe") != std::string::npos)
					{
						TerminateProcess(processHandle, 1); //doesnt seem to work?
						CloseHandle(processHandle);
						return { str.begin(), str.end() };
					}
				}
				CloseHandle(processHandle);
			}
		}
	}
	return "";
}

#endif //#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)


//#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
void ResourcesLoadingLayer::handleMac() {}
//#endif

//#if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
void ResourcesLoadingLayer::handleLinux() {
	GameToolbox::log("Linux resource handler started");
	
	auto label = Label::create();
	label->setString("Geometry Dash Resources not found!\nSearching for resources...");
	label->setSystemFontSize(20);
	label->setPosition(_posMiddle);
	addChild(label);

	// Log current search paths
	GameToolbox::log("Current search paths:");
	for (const auto& path : _fu->getSearchPaths()) {
		GameToolbox::log("  {}", path);
	}

	// Check configured path
	std::string resourcePath = std::string(Config::getResourcesPath()) + "/Resources";
	GameToolbox::log("Checking configured path: {}", resourcePath);
	
	// Validate required files and directories
	std::vector<std::string> requiredFiles = {
		"game_bg_01_001-hd.png",
		"GJ_LaunchSheet-hd.png",
		"GJ_GameSheet03-uhd.png"
	};

	std::vector<std::string> requiredDirs = {
		"levels"
	};

	bool hasAllFiles = true;
	
	// Check required files
	for (const auto& file : requiredFiles) {
		std::string fullPath = resourcePath + "/" + file;
		if (!_fu->isFileExist(fullPath)) {
			GameToolbox::log("Missing required file: {}", fullPath);
			hasAllFiles = false;
		}
	}

	// Check required directories
	for (const auto& dir : requiredDirs) {
		std::string fullPath = resourcePath + "/" + dir;
		if (!_fu->isDirectoryExist(fullPath)) {
			GameToolbox::log("Missing required directory: {}", fullPath);
			hasAllFiles = false;
		}
	}

	// If levels directory doesn't exist, don't proceed
	if (!_fu->isDirectoryExist(resourcePath + "/levels")) {
		GameToolbox::log("Critical error: levels directory not found");
		label->setString("Error: levels directory not found.\nPlease check OPENGD_RESOURCES environment variable\nand ensure it points to a valid Geometry Dash installation.");
		return;
	}

	// Create Custom directory if it doesn't exist
	std::string customDir = "Custom";
	if (!_fu->isDirectoryExist(customDir)) {
		GameToolbox::log("Creating Custom directory");
		_fu->createDirectory(customDir);
	}

	// Create or validate mainLevels.json
	std::string levelsPath = "Custom/mainLevels.json";
	if (!_fu->isFileExist(levelsPath)) {
		GameToolbox::log("Creating mainLevels.json");
		
		// Create JSON with level data from level files
		nlohmann::json levelsJson;
		
		// Check for level files in the levels directory
		std::string levelsDir = resourcePath + "/levels";
		
		// Get all .txt files in the levels directory
		for (int i = 1; i <= 22; i++) {
			std::string levelFile = levelsDir + "/" + std::to_string(i) + ".txt";
			if (_fu->isFileExist(levelFile)) {
				std::string levelData = _fu->getStringFromFile(levelFile);
				if (!levelData.empty()) {
					// Remove the H4sIAAAAAAAAA prefix if it exists
					if (levelData.substr(0, 14) == "H4sIAAAAAAAAA") {
						levelData = levelData.substr(14);
					}
					levelsJson[std::to_string(i)] = levelData;
					GameToolbox::log("Added level {} to mainLevels.json", i);
				}
			}
		}
		
		// If no levels were found, don't create an empty file
		if (levelsJson.empty()) {
			GameToolbox::log("No level files found in levels directory");
			label->setString("Error: No level files found in levels directory.\nPlease check your Geometry Dash installation.");
			return;
		}
		
		std::string jsonStr = levelsJson.dump(4);
		_fu->writeStringToFile(jsonStr, levelsPath);
	} else {
		// Validate existing JSON
		try {
			std::string content = _fu->getStringFromFile(levelsPath);
			if (content.empty()) {
				GameToolbox::log("Empty JSON file: {}", levelsPath);
				hasAllFiles = false;
			} else {
				nlohmann::json::parse(content);  // Validate JSON format
			}
		} catch (const std::exception& e) {
			GameToolbox::log("Invalid JSON file {}: {}", levelsPath, e.what());
			hasAllFiles = false;
		}
	}

	if (_fu->isDirectoryExist(resourcePath) && hasAllFiles) {
		GameToolbox::log("Found valid resource path with all required files: {}", resourcePath);
		_fu->addSearchPath(resourcePath, true);
		_gm->set<std::string>("resources_path", resourcePath);
		_gm->save();
		loadLoadingLayer();
		return;
	}

	GameToolbox::log("No valid resource path found or missing required files");
	label->setString("Resources not found or incomplete.\nPlease check OPENGD_RESOURCES environment variable\nand ensure it points to a valid Geometry Dash installation.");
}
//#endif