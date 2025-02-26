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
#include "2d/Layer.h"

class SimpleProgressBar;

namespace ax 
{ 
	class Scene; 
	class SpriteFrameCache;
	class TextureCache;
}


class LoadingLayer : public ax::Layer {
private:
	void loadIcons();

	int getPlayerIconsSize();
	int getShipIconsSize();
	int getPlayerBallIconsSize();
	int getBirdIconsSize();
public:
	const char* getSplash();
	static ax::Scene* scene();
	static LoadingLayer* create();
	bool init();
	void loadAssets();
	void assetLoaded(ax::Object*);
	SimpleProgressBar* _pBar;
	
	float m_nAssetsLoaded = 0;
	float m_nTotalAssets = 0;
	
	//avoid always calling getInstance for these
	ax::SpriteFrameCache* _sprFrameCache;
	ax::TextureCache* _textureCache;

};
